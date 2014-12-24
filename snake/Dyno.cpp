#include "Dyno.h"

std::vector<std::string> dyno::splitString(std::string& str)
{
    std::vector<std::string> words;

    size_t start_pos = 0, end_pos = 0;

    while(end_pos != std::string::npos)
    {
        end_pos = str.find(' ', start_pos);

        //if at end, use length = maxlength else use length = end-start
        std::string string_to_push = str.substr(start_pos,
            (end_pos == std::string::npos) ? std::string::npos : end_pos - start_pos);
        //cout << "push: " << string_to_push << "\n";
        words.push_back(string_to_push);

        start_pos = ( ( end_pos > (std::string::npos - 1)) ? std::string::npos : end_pos + 1);
    }
    return words;
}
