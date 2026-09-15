#include <SDL3/SDL_log.h>

#include "JSONParser.h"
#include "FontManager.h"
#include "DataProvider.h"
#include "IOMiscs.h"
#include "FNV1aHash.h"
#include "FloatUtils.h"

using json = nlohmann::json;

/**
* @brief Evaluates an expression, replacing defines and calculating.
*
* @param expression String containing the expression.
* @param defines JSON containing the defines.
* @param [out] success 'true' if the evaluation was successfull, 'false' otherwise.
*
* @return The value of the expression, or '0.0' if something has failed.
*/
static double EvaluateExpression(const std::string& expression, const json& defines, bool& success);

/**
* @brief Recursive string-replacer for definition parsing.
*
* WILL MODIFY THE OBJECT!
*
* @param [out] node Current node of the JSON file.
* @param definitions The JSON containing the defines.
*/
static void ResolveNode(json& node, const json& defines);

/**
* @brief Parses a single system action.
* 
* @param eventJSON JSON containing the action data.
* 
* @return The respective SystemAction of the JSON, or std::nullopt if no action matches.
*/
std::optional<SystemAction> ParseSystemAction(const nlohmann::json& eventJson);

void JSONParser::ParseJSONDefines(json& root, JSONParseFlags shouldDeleteDefines)
{
    if (root.contains("@defines") && root["@defines"].is_object())
    {
        json definitions = root["@defines"];

        ResolveNode(root, definitions);

        if (HasFlag(shouldDeleteDefines, JSONParseFlags::DeleteDefines))
        {
            root.erase("@defines");
        }
    }
}

SDL_Color JSONParser::ParseJSONColor(const json& colorJSON, const SDL_Color& def)
{
    /*
        What this function expects:

        Note that colorJSON is the default JSON object / array / string.

        [255, 13, 245, 128]

        { "r": 13, "g": 4, "b": 255, "a": 255 }

        "value": "0xFF00FF"

        "value": "#AAA"

        "value": "00AA09h"
    */

    if (colorJSON.is_array() && (colorJSON.size() == 3 || colorJSON.size() == 4))
    {
        return SDL_Color {
            colorJSON.at(0).get<uint8_t>(),
            colorJSON.at(1).get<uint8_t>(),
            colorJSON.at(2).get<uint8_t>(),
            (colorJSON.size() == 4) ? colorJSON.at(3).get<uint8_t>() : (uint8_t)255
        };
    }
    else if (colorJSON.is_object())
    {
        if (!colorJSON.contains("r") || !colorJSON["r"].is_number_unsigned() ||
            !colorJSON.contains("g") || !colorJSON["g"].is_number_unsigned() ||
            !colorJSON.contains("b") || !colorJSON["b"].is_number_unsigned() ||
            !colorJSON.contains("a") || !colorJSON["a"].is_number_unsigned())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONColor: Color JSON object doesn't contain r, g, b or a or not unsigned values. Defaulting to 255.");
        }

        return SDL_Color {
            colorJSON.value("r", (uint8_t)255),
            colorJSON.value("g", (uint8_t)255),
            colorJSON.value("b", (uint8_t)255),
            colorJSON.value("a", (uint8_t)255)
        };
    }
    else if (colorJSON.is_string())
    {
        const std::string& colorString = colorJSON.get_ref<const std::string&>();

        const bool startsWith0x = colorString.starts_with("0x") || colorString.starts_with("0X");
        const bool startsWithHash = colorString.starts_with("#");
        const bool endsWithH = colorString.ends_with("h") || colorString.ends_with("H");

        if (!startsWith0x && !startsWithHash && !endsWithH)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONColor: Color JSON is string [%s] but doesn't start with '0x', '#' or end with 'h'.", colorString.c_str());
            return def;
        }

        std::string hexValueStr;

        if (startsWith0x)
        {
            hexValueStr = colorString.substr(2);
        }
        else if (startsWithHash)
        {
            hexValueStr = colorString.substr(1);
        }
        else // endsWithH
        {
            hexValueStr = colorString.substr(0, colorString.size() - 1);
        }

        if (hexValueStr.size() != 3 && hexValueStr.size() != 6)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONColor: Color JSON is hex string [%s] but has [%llu] characters, instead of 3 or 6.", hexValueStr.c_str(), hexValueStr.size());
            return def;
        }

        // I hate try-catch, but stoi throws errors.
        try
        {
            if (hexValueStr.size() == 3)
            {
                std::string rStr = hexValueStr.substr(0, 1);
                std::string gStr = hexValueStr.substr(1, 1);
                std::string bStr = hexValueStr.substr(2, 1);

                uint8_t r = static_cast<uint8_t>(std::stoi(rStr + rStr, nullptr, 16));
                uint8_t g = static_cast<uint8_t>(std::stoi(gStr + gStr, nullptr, 16));
                uint8_t b = static_cast<uint8_t>(std::stoi(bStr + bStr, nullptr, 16));

                return SDL_Color { r, g, b, 255 };
            }
            else // size == 6
            {
                uint8_t r = static_cast<uint8_t>(std::stoi(hexValueStr.substr(0, 2), nullptr, 16));
                uint8_t g = static_cast<uint8_t>(std::stoi(hexValueStr.substr(2, 2), nullptr, 16));
                uint8_t b = static_cast<uint8_t>(std::stoi(hexValueStr.substr(4, 2), nullptr, 16));

                return SDL_Color { r, g, b, 255 };
            }
        }
        catch (const std::exception& e)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONColor: Failed to parse hex values [%s]. Error: %s", hexValueStr.c_str(), e.what());
            return def;
        }
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONColor: Color JSON not object, array or string.");
    return def;
}

std::vector<SystemAction> JSONParser::ParseJSONSystemActions(const nlohmann::json& eventJSON)
{
    std::vector<SystemAction> actions;

    if(eventJSON.is_null())
    {
        return actions;
    }
    else if (eventJSON.is_array())
    {
        for (const json& item : eventJSON)
        {
            if (std::optional<SystemAction> act = ParseSystemAction(item))
            {
                actions.push_back(*act);
            }
        }
        return actions;
    }
    else if (eventJSON.is_object())
    {
        if (std::optional<SystemAction> act = ParseSystemAction(eventJSON))
        {
            actions.push_back(*act);
        }
        return actions;
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONSystemActions: EventJSON is not null, array or object.");
    return actions;
}

UITextAlignment JSONParser::ParseJSONAlignment(const nlohmann::json& alignmentJSON, const UITextAlignment& def)
{
    /*
        What this function expects:

        Note that alignmentJSON is the default JSON object:

        { 
            "horizontal": "center", 
            "vertical": "middle" 
        }
    */

    if (!alignmentJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAlignment: JSON is not object.");
        return def;
    }

    if (!alignmentJSON.contains("horizontal") || !alignmentJSON["horizontal"].is_string() ||
        !alignmentJSON.contains("vertical") || !alignmentJSON["vertical"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAlignment: Aligment JSON doesn't contain horizontal or vertical alignment, or they are not strings. Defaulting to {center, middle}.");
    }

    const std::string& horizontalAlignmentStr = alignmentJSON.value("horizontal", "center");
    const std::string& verticalAlignmentStr = alignmentJSON.value("vertical", "middle");

    HorizontalAlignment horizontalAlignment = HorizontalAlignmentFromString(horizontalAlignmentStr);
    VerticalAlignment veritcalAlignment = VerticalAlignmentFromString(verticalAlignmentStr);

    if (IsSentinel(horizontalAlignment))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAlignment: Alignment JSON text alignment for horizontal alignment [%s] invalid. Defaulting to \'def\'.", horizontalAlignmentStr.c_str());
        horizontalAlignment = def.horizontalAlignment;
    }

    if (IsSentinel(veritcalAlignment))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAlignment: Alignment JSON text alignment for vertical alignment [%s] invalid. Defaulting to \'def\'.", verticalAlignmentStr.c_str());
        veritcalAlignment = def.verticalAlignment;
    }

    return UITextAlignment { .horizontalAlignment = horizontalAlignment, .verticalAlignment = veritcalAlignment };
}

UITextStyleElement JSONParser::ParseJSONTextStyle(const nlohmann::json& styleJSON, const UITextStyleElement& def)
{
    /*
        What this function expects:

        Note that styleJSON is the default JSON object.

        {
            "fontFamily": "button",
            "fontSize": 24.0,
            "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
            "textAlignment": { "horizontal": "center", "vertical": "middle" }
        }
    */

    if (!styleJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTextStyle: JSON is not object.");
        return def;
    }

    std::string fontFamily = styleJSON.value("fontFamily", def.GetFontFamily());
    float fontSize = def.GetOriginalFontSize();

    if (styleJSON.contains("fontSize") && styleJSON["fontSize"].is_number())
    {
        fontSize = static_cast<float>(styleJSON.at("fontSize").get<double>());
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTextStyle: 'fontSize' missing or is not a number. Falling back to default.");
    }

    TTF_Font* textFont = FontManager::GetInstance().GetFontByFamily(fontFamily, fontSize);
    if (!textFont)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTextStyle: Failed to retrieve font for family [%s] with size [%f]. Falling back to default font.", fontFamily.c_str(), fontSize);
        textFont = def.GetTextFont();
    }

    SDL_Color fontColor = def.GetTextColor();
    if (styleJSON.contains("fontColor") && (styleJSON["fontColor"].is_object() || styleJSON["fontColor"].is_array() || styleJSON["fontColor"].is_string()))
    {
        fontColor = ParseJSONColor(styleJSON["fontColor"], def.GetTextColor());
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTextStyle: 'fontColor' field is not an object, array or string.");
    }

    UITextAlignment textAlignment = def.GetTextAlignment();
    if (styleJSON.contains("textAlignment") )
    {
        if (styleJSON["textAlignment"].is_object())
        {
            textAlignment = ParseJSONAlignment(styleJSON["textAlignment"], def.GetTextAlignment());
        }
        else if (!styleJSON["textAlignment"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTextStyle: 'textAlignment' field exists, but is not null or object.");
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTextStyle: 'textAlignment' field missing.");
    }

    return UITextStyleElement{ textFont, fontColor, textAlignment, fontFamily };
}

UIElementProperties JSONParser::ParseJSONUIElementProps(const nlohmann::json& elementJSON, const UIElementProperties& def)
{
    /*
        What this function expects:

        Note that elementJSON is the default JSON object.

        {
            "bounds": [100.0, 200.0, 30.0, 40.0],
            "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
            "borderColor": { "r": 255, "g": 255, "b": 255, "a": 255 }
        }
    */

    if (!elementJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONUIElementProps: JSON is not object.");
        return def;
    }

    SDL_FRect bounds = def.bounds;
    if (elementJSON.contains("bounds") && (elementJSON["bounds"].is_object() || elementJSON["bounds"].is_array()))
    {
        bounds = ParseJSONRect<SDL_FRect>(elementJSON["bounds"], def.bounds);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONUIElementProps: 'bounds' field missing or not object or array. Defaulting to fallback.");
    }

    SDL_Color backgroundColor = def.backgroundColor;
    if (elementJSON.contains("backgroundColor") && (elementJSON["backgroundColor"].is_object() || elementJSON["backgroundColor"].is_array() || elementJSON["backgroundColor"].is_string()))
    {
        backgroundColor = ParseJSONColor(elementJSON["backgroundColor"], def.backgroundColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONUIElementProps: 'backgroundColor' field missing or is not an object, array, or string.");
    }

    SDL_Color borderColor = def.borderColor;
    if (elementJSON.contains("borderColor") && (elementJSON["borderColor"].is_object() || elementJSON["borderColor"].is_array() || elementJSON["borderColor"].is_string()))
    {
        borderColor = ParseJSONColor(elementJSON["borderColor"], def.borderColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONUIElementProps: 'borderColor' field missing or is not an object, array, or string.");
    }

    return UIElementProperties{ bounds, backgroundColor, borderColor };
}

std::vector<std::vector<std::string>> JSONParser::ParseJSONTableDatasource(const nlohmann::json& sourceJSON, const std::vector<std::vector<std::string>>& def)
{
    /*
        What this function expects:


        Note that sourceJSON is the default JSON object.

        {
            "type": "provider",
            "key": "topScores"
        }

        { 
            "type": "file", 
            "path": "resources/data/scores.csv" 
        }

        { 
            "type": "inline", 
            "data": [
                ["Alice", "5", "10:20"], 
                ["Bob", "2", "04:12"]
            ] 
        }
    */

    if (!sourceJSON.contains("type") || !sourceJSON["type"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTableDatasource: Missing type, defaulting to 'inline'.");
    }

    std::string type = sourceJSON.value("type", "inline");

    if (type == "provider")
    {
        if (!sourceJSON.contains("key") || !sourceJSON["key"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTableDatasource: Type is 'provider', but key is missing or not string.");
            return def;
        }

        std::string key = sourceJSON.value("key", "");
        std::optional<std::vector<std::vector<std::string>>> dataOpt = DataProvider::GetInstance().FetchData<std::vector<std::vector<std::string>>>(key);

        if (!dataOpt)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTableDatasource: Type is 'provider', but key isn't registered.");
            return def;
        }

        return *dataOpt;
    }
    else if (type == "file")
    {
        std::string path = sourceJSON.value("path", "");
        //return LoadCSVOrJSONData(path);
        throw std::exception("Do this!!!!!!!!!");
    }
    else if (type == "inline")
    {
        return sourceJSON.value("data", std::vector<std::vector<std::string>>{});
    }

    return {};
}

SDL_FPoint JSONParser::ParseJSONPadding(const nlohmann::json& paddingJSON, const SDL_FPoint& def)
{
    /*
        What this function expects:

        "padding": 40
        "padding": { "x": 40, "y": 20 }
        "padding": { "horizontal": 40, "vertical": 20 }
    */

    if (paddingJSON.is_number())
    {
        float p = paddingJSON.get<float>();
        return SDL_FPoint { p, p };
    }

    if (paddingJSON.is_object())
    {
        SDL_FPoint padding = def;

        if (paddingJSON.contains("x") && paddingJSON["x"].is_number())
        {
            padding.x = paddingJSON.at("x").get<float>();
        }
        else if (paddingJSON.contains("horizontal") && paddingJSON["horizontal"].is_number()) 
        {
            padding.x = paddingJSON.at("horizontal").get<float>();
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONPadding: Padding is object but has no 'x' or 'horizontal'. Defaulting to \"def.x\"");
        }

        if (paddingJSON.contains("y") && paddingJSON["y"].is_number()) 
        {
            padding.y = paddingJSON.at("y").get<float>();
        }
        else if (paddingJSON.contains("vertical") && paddingJSON["vertical"].is_number()) 
        {
            padding.y = paddingJSON.at("vertical").get<float>();
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONPadding: Padding is object but has no 'y' or 'vertical'. Defaulting to \"def.y\"");
        }

        return padding;
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONPadding: Failed to properly parse padding, either not object or not number. Returning \"def\"");
    return def;
}

DialogueData JSONParser::ParseJSONDialogueData(const nlohmann::json& dataJSON, const DialogueData& def)
{
    /*
        What this function expects:

        Note that dataJSON is the default JSON object

        {
          "type": "choice",
          "question": "What are you doing here?",
          "choices": [
            {
              "bankFile": "responsesBad.json",
              "count": 3,
              "randomize": true,
              "actions": [...]
            },
            {
              "bankFile": "responsesGood.json",
              "count": 1,
              "randomize": true,
              "actions": [...]
            }
          ]
        }

        {
          "type": "narration",
          "lines": {
            "bankFile": "NPC1_dialogue"
          }
        }

        "value": "node.json"
    */

    DialogueData data;

    if (dataJSON.is_string())
    {
        data.type = DialogueType::Node;
        data.data = dataJSON.get<std::string>();
    }
    else if (dataJSON.is_object())
    {
        if (!dataJSON.contains("type") || !dataJSON["type"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONDialogueData: Data JSON does not contain \"type\" or is not string.");
            return def;
        }

        data.type = DialogueTypeFromString(dataJSON.at("type").get_ref<const std::string&>());

        data.data = dataJSON;
        data.data.erase("type");
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONDialogueData: Data JSON is not string or object.");
        return def;
    }

    return data;
}

AudioCommand JSONParser::ParseJSONAudioCommand(const nlohmann::json& audioJSON, const AudioCommand& def)
{
    /*
        What this function expects:

        Note that audioJSON is the default JSON object.

        {
            "target": {
                "type": "clip",
                "id": "map1Music"
            },
            "action": "play"
            "playback": {
                "loop": true,
                "loopCount": 3,
                "fadeIn": 1000,
                "volume": 0.5
            }
        }

        {
            "target": {
                "type": "tag",
                "id": "SFX"
            },
            "action": "stop",
            "fadeOut": 500
        }

        {
            "target": {
                "type": "all"
            },
            "action": "pause"
        }
    */

    AudioCommand command;

    if (!audioJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Expected object, got non-object.");
        return def;
    }

    if (!audioJSON.contains("target") || !audioJSON["target"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Target for audio control is not object or doesn't exist.");
        return def;
    }

    if (!audioJSON.contains("action") || !audioJSON["action"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Action for audio control doesn't exist or is not string.");
        return def;
    }

    const json& target = audioJSON["target"];
    const std::string& actionStr = audioJSON.at("action").get_ref<const std::string&>();
    AudioAction action = AudioActionFromString(actionStr);

    if (IsSentinel(action))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Invalid action [%s].", actionStr.c_str());
        return def;
    }

    command.action = action;

    if (!target.contains("type") || !target["type"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Target type missing or is not string.");
        return def;
    }

    const std::string& typeStr = target.at("type").get_ref<const std::string&>();
    AudioTargetType type = AudioTargetTypeFromString(typeStr);

    if (IsSentinel(type))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Invalid target type [%s].", typeStr.c_str());
        return def;
    }

    command.targetType = type;

    if (type != AudioTargetType::All && (!target.contains("id") || !target["id"].is_string()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Missing target ID for tag or clip target.");
        return def;
    }

    if (command.targetType != AudioTargetType::All)
    {
        const std::string& id = target.at("id").get_ref<const std::string&>();
        command.targetID = FNV1aHash::HashString(id);
    }

    if (command.action == AudioAction::Stop)
    {
        if (audioJSON.contains("fadeOut") && (audioJSON["fadeOut"].is_number() || audioJSON["fadeOut"].is_object()))
        {
            command.opts.fadeMs = JSONParser::ParseJSONTime<int64_t>(audioJSON["fadeOut"], 0);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Action is 'stop' but missing fade out ms, or is object or number. Defaulting to no fade out.");
        }
    }

    if (command.action == AudioAction::Play)
    {
        if (audioJSON.contains("playback") && audioJSON["playback"].is_object())
        {
            command.opts.playbackOverride = JSONParser::ParseJSONAudioPlaybackOptions(audioJSON["playback"]);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Action is 'play' but missing playback options or is not an object. Defaulting to no override.");
        }
    }

    if (command.action == AudioAction::PlayOrResume && command.targetType != AudioTargetType::Clip)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioCommand: Action is 'playOrResume' but target type is not clip.");
    }

    return command;
}

ScoreEntry JSONParser::ParseJSONScoreEntry(const nlohmann::json& scoreJSON, const ScoreEntry& def)
{
    /*
        What this function expects:

        Note that scoreJSON is the defualt JSON object.

        { 
            "name": "PlayerOne", 
            "caught": 2, 
            "time": 145 
        }
    */

    if (!scoreJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONScoreEntry: Score entry is not object.");
        return def;
    }

    if (!scoreJSON.contains("name") || !scoreJSON["name"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONScoreEntry: Score entry does not contain name or is not a string.");
        return def;
    }

    const std::string& name = scoreJSON.at("name").get_ref<const std::string&>();

    if (!scoreJSON.contains("caught") || !scoreJSON["caught"].is_number_integer())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONScoreEntry: Score entry for name [%s] does not contain the number of times caught or is not integer.", name.c_str());
        return def;
    }

    if (!scoreJSON.contains("time") || !scoreJSON["time"].is_number_integer())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONScoreEntry: Score entry for name [%s] does not contain the time or is not integer.", name.c_str());
        return def;
    }

    const int caught = scoreJSON.at("caught").get<int>();
    const uint64_t time = scoreJSON.at("time").get<uint64_t>();

    return ScoreEntry{
        .name = name,
        .timesCaught = caught,
        .timePlayed = time
    };
}

AudioPlaybackOptions JSONParser::ParseJSONAudioPlaybackOptions(const nlohmann::json& playbackJSON, const AudioPlaybackOptions& def)
{
    /*
        What this function expects:

        Note that 'playbackJSON' is the default JSON object

        {
            "loop": true,
            "loopCount": 3,
            "fadeInMs": 1000,
            "volume": 0.5
        }
    */

    AudioPlaybackOptions opts;

    if (!playbackJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Expected object, got non-object.");
        return def;
    }

    if (!playbackJSON.contains("loop") || !playbackJSON["loop"].is_boolean())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Playback options doesn't contain loop or is not boolean. Defaulting to no looping.");
    }

    const bool loop = playbackJSON.value("loop", false);
    int64_t loopCount = -1;

    if (playbackJSON.contains("loopCount") && playbackJSON["loopCount"].is_number())
    {
        if (loop)
        {
            loopCount = playbackJSON.value("loopCount", -1);
        }
        else if(!playbackJSON["loopCount"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Audio entry is set to NOT loop but contains loop count.");
        }
    }
    else if (loop)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Playback options doesn't contain loop count or is not integer, but is set to loop. Defaulting to infinite loops.");
    }

	int64_t fadeInMs = 0;

    if (playbackJSON.contains("fadeIn"))
    {
        if (playbackJSON["fadeIn"].is_number() || playbackJSON["fadeIn"].is_object())
        {
            fadeInMs = JSONParser::ParseJSONTime<int64_t>(playbackJSON["fadeIn"], fadeInMs);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Playback options contains \"fadeIn\" or is not number of object. Defaulting to 0.");
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Playback options doesn't contain \"fadeIn\". Defaulting to 0.");
    }

    if (!playbackJSON.contains("volume") || !playbackJSON["volume"].is_number())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONAudioPlaybackOptions: Playback options doesn't contain volume or is not float. Defaulting to 1.0f.");
    }

    float volume = playbackJSON.value("volume", 1.0f);

    opts.SetFadeInMs(fadeInMs < 0 ? 0 : fadeInMs);
    if (loop)
    {
        opts.SetLoopCount(loopCount < 0 ? -1 : loopCount);
    }
    opts.SetVolume(FloatUtils::IsLessF(volume, 0.0f) ? 0.0f : volume);

    return opts;
}

std::filesystem::path JSONParser::ParseJSONPath(const nlohmann::json& pathJSON, const std::filesystem::path& def)
{
    /*
        What this function expects:

        "value": <path as string>
    */

    if (pathJSON.is_string())
    {
        return IOMiscs::MakeCrossPlatformPath(pathJSON.get_ref<const std::string&>());
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONPath: Given JSON is not string. Returning 'def'.");
    return def;
}

static double EvaluateExpression(const std::string& expression, const json& defines, bool& success)
{
    success = false;
    std::vector<double> values;
    std::vector<char> ops;

    std::stringstream ss(expression);
    std::string token;

    // I love building math parsers for JSON.
    while (ss >> token)
    {
        // It's a token.
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            ops.push_back(token[0]);
        }
        else
        {
            double val = 0.0;
            if (defines.contains(token) && defines[token].is_number())
            {
                // It's a define / macro variable. (ex: _DEFAULT_SIZE)
                val = defines.at(token).get<double>();
            }
            else
            {
                // It's a number (ex: 3)
                try
                {
                    val = std::stod(token);
                }
                catch (const std::exception& e)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "EvaluateExpression: Failed to parse token [%s] for expression [%s], error: %s", token.c_str(), expression.c_str(), e.what());
                    return 0.0;
                }
            }
            values.push_back(val);
        }
    }

    // There should be one more value (operand) than operators.
    if (values.size() != ops.size() + 1 || values.empty())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "EvaluateExpression: Empty value vector or number of values [%llu] is not exactly one more than number of operators [%llu] for expression: %s", values.size(), ops.size(), expression.c_str());
        return 0.0;
    }

    // VBR, 2 pass.
    // First pass; * and /
    for (size_t i = 0; i < ops.size(); )
    {
        if (ops[i] == '*' || ops[i] == '/')
        {
            if (ops[i] == '*')
            {
                values[i] = values[i] * values[i + 1];
            }
            else
            {
                if (values[i + 1] == 0.0)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "EvaluateExpression: Division by zero in expression: %s", expression.c_str());
                    return 0.0;
                }
                values[i] = values[i] / values[i + 1];
            }
            // Erase the consumed right-hand value and operator.
            values.erase(values.begin() + i + 1);
            ops.erase(ops.begin() + i);
        }
        else
        {
            // Only increment if we didn't shrink the vectors.
            i++;
        }
    }

    // Second pass; + and -
    double result = values[0];
    for (size_t i = 0; i < ops.size(); ++i)
    {
        if (ops[i] == '+')
        {
            result += values[i + 1];
        }

        if (ops[i] == '-')
        {
            result -= values[i + 1];
        }
    }

    success = true;
    return result;
}

static void ResolveNode(json& node, const json& defines)
{
    if (node.is_string())
    {
        const std::string& key = node.get_ref<const std::string&>();

        // Direct exact match check.
        if (defines.contains(key))
        {
            node = defines[key];
            return;
        }

        // Check if it's a math expression containing any math operators.
        if (key.find_first_of("+-*/") != std::string::npos)
        {
            bool success = false;
            double result = EvaluateExpression(key, defines, success);

            if (success)
            {
                // Assign as integer if it's whole, otherwise keep double.
                if (result == static_cast<long long>(result))
                {
                    node = static_cast<long long>(result);
                }
                else
                {
                    node = result;
                }
            }
        }
    }
    else if (node.is_object())
    {
        for (auto& [key, value] : node.items())
        {
            ResolveNode(value, defines);
        }
    }
    else if (node.is_array())
    {
        for (json& element : node)
        {
            ResolveNode(element, defines);
        }
    }
}

std::optional<SystemAction> ParseSystemAction(const nlohmann::json& eventJson)
{
    /*
        What this function expects:

        Note that eventJSON is the default JSON object:

        {
            "changePanel": "main"
        }

        {
            "changeStyle": {
                "backgroundColor": [255, 0, 61, 255]
            }
        }

        {
            "exitGame": null
        }

        {
            "setVolume": 0.8
        }

        {
            "playCutscene": "prologue",
        }
    */

    if (!eventJson.is_object() || eventJson.empty())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseSystemAction: Provided JSON is not a valid event object.");
        return std::nullopt;
    }

    auto it = eventJson.begin();

    SystemAction action;
    action.type = it.key();
    action.payload = it.value();

    return action;
}
