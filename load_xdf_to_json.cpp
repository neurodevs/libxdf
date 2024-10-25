#include "xdf.h"
#include <json.hpp>
#include <string>
#include <vector>
#include <sstream>

using json = nlohmann::json;

extern "C" char* load_xdf_to_json(const char* filename) {
    Xdf xdf_instance;
    std::string filePath(filename);

    xdf_instance.load_xdf(filePath);

    json root;
    root["stream_count"] = static_cast<int>(xdf_instance.streams.size());

    for (size_t i = 0; i < xdf_instance.streams.size(); i++) {
        const Xdf::Stream& stream = xdf_instance.streams[i];
        json streamJson;
        streamJson["stream_id"] = static_cast<int>(i);

        json infoJson;
        infoJson["stream_name"] = stream.info.name;
        infoJson["stream_type"] = stream.info.type;
        infoJson["channel_count"] = stream.info.channel_count;
        infoJson["nominal_srate"] = stream.info.nominal_srate;
        infoJson["channel_format"] = stream.info.channel_format;

        streamJson["stream_info"] = infoJson;

        for (size_t j = 0; j < stream.time_series.size(); j++) {
            json channelJson;

            for (size_t k = 0; k < stream.time_series[j].size(); k++) {
                channelJson.push_back(stream.time_series[j][k]);
            }
            
            streamJson["time_series"].push_back(channelJson);
        }

        for (size_t j = 0; j < stream.time_stamps.size(); j++) {
            streamJson["time_stamps"].push_back(stream.time_stamps[j]);
        }

        root["streams"].push_back(streamJson);
    }

    const std::vector<std::pair<std::pair<std::string, double>, int>> eventMap = xdf_instance.eventMap;

    json eventJson;
    
    for (size_t i = 0; i < eventMap.size(); i++) {
        const std::pair<std::pair<std::string, double>, int>& event = eventMap[i];
        json eventItem;
        eventItem["stream_id"] = event.second;
        eventItem["event_name"] = event.first.first;
        eventItem["event_timestamp"] = event.first.second;
        eventJson.push_back(eventItem);
    }

    root["events"] = eventJson;

    std::string serialized_data = root.dump();
    char* cstr = new char[serialized_data.length() + 1];
    std::strcpy(cstr, serialized_data.c_str());

    return cstr;
}

extern "C" void free_string(char* str) {
    delete[] str;
}
