#include "body.h"

void parseBody(const pugi::xml_node &body){
    for(pugi::xml_node node: body.children()){
        if(std::string_view(node.name()) == "joint"){
            
        }
    }
}