#include "chaos_engine.h"

ChaosEngine::ChaosEngine(char (&front_to_back_buff)[], char (&back_to_front_buff)[], bool& request, bool& response, int& ftbbytes, int& ftb_offset, int& btfbytes, int& btf_offset)
    : request(request), response(response), ftbbytes(ftbbytes), ftb_offset(ftb_offset), btfbytes(btfbytes), btf_offset(btf_offset){
    
    this->front_to_back_buff = front_to_back_buff;
    this->back_to_front_buff =  back_to_front_buff;
    
    try{ // TODO move parsing of json file in Connection class, pass j 
        f.open("config.json");
        j = json::parse(f);
        f.close();
    }catch(exception& e){
        cerr << e.what() << endl;
    }
    register_all();
}

ChaosEngine::~ChaosEngine(){}

void ChaosEngine::chunk_drop(){
    if (!j.at("chaos").at("chunk_drop").at("enabled")){return;}
    bernoulli_distribution d(j.at("chaos").at("chunk_drop").at("rate"));

    if(request && (j.at("chaos").at("chunk_drop").at("direction") == "request" || j.at("chaos").at("chunk_drop").at("direction") == "both")){
        if (d(gen) && ftbbytes > 0){
            int drop = ftbbytes / 3; //drop a third of the chunk. Could also use rand on division 
            ftb_offset += drop;
            ftbbytes  -= drop;
        }
    }else if (response && (j.at("chaos").at("chunk_drop").at("direction") == "response" || j.at("chaos").at("chunk_drop").at("direction") == "both")){
        if (d(gen) && btfbytes > 0){
            int drop = btfbytes / 3;
            btf_offset += drop;
            btfbytes  -= drop;
        }
    }


}

void ChaosEngine::directional_drop(){
    // https://en.cppreference.com/cpp/numeric/random/bernoulli_distribution
    if (!j.at("chaos").at("directional_drop").at("enabled")){return;}
    
    bernoulli_distribution d(j.at("chaos").at("directional_drop").at("rate"));

    if(request && (j.at("chaos").at("directional_drop").at("direction") == "request" || j.at("chaos").at("directional_drop").at("direction") == "both")){ // front sent data
        if (d(gen)){
            // cout << "request: Triggered Directional Drop " << request <<endl;
            ftbbytes = 0;
            ftb_offset = 0;
        }
    }else if (response && (j.at("chaos").at("directional_drop").at("direction") == "response" || j.at("chaos").at("directional_drop").at("direction") == "both")){ //received data from back
        if (d(gen)){
            // cout << "response: Triggered Directional Drop " << response << endl;
            btfbytes = 0;
            btf_offset = 0;
        }
    }
}


void ChaosEngine::byte_corruption(){
    if (!j.at("chaos").at("byte_corruption").at("enabled")){return;}
    
    bernoulli_distribution d(j.at("chaos").at("byte_corruption").at("rate"));

    if(request && (j.at("chaos").at("byte_corruption").at("direction") == "request" || j.at("chaos").at("byte_corruption").at("direction") == "both")){ // front sent data
        for(long unsigned int i = 0; i< strlen(front_to_back_buff); i++){
            if(d(gen)){
                front_to_back_buff[i] = '%';
            }
        }
    }else if (response && (j.at("chaos").at("byte_corruption").at("direction") == "response" || j.at("chaos").at("byte_corruption").at("direction") == "both")){ //received data from back
        for(long unsigned int i = 0; i< strlen(back_to_front_buff); i++){
            if(d(gen)){
                back_to_front_buff[i] = '$';
            }
        }
    }
}
void ChaosEngine::bit_flipping(){}
void ChaosEngine::header_corruption(){}
void ChaosEngine::body_corruption(){}

void ChaosEngine::fixed_delay(){}
void ChaosEngine::random_delay(){}


void ChaosEngine::header_removal(){}
void ChaosEngine::header_duplication(){}
void ChaosEngine::wrong_content_length(){}
void ChaosEngine::status_manipulation(){}

void ChaosEngine::logs(){}

void ChaosEngine::register_all(){ //TODO lookup Static Auto-Registration 
        registry["chunk_drop"] = [this]() { chunk_drop(); };
        registry["directional_drop"] = [this]() { directional_drop(); };
        registry["byte_corruption"] = [this]() { byte_corruption(); };
        registry["bit_flipping"] = [this]() { bit_flipping(); };
        registry["fixed_delay"] = [this]() { fixed_delay(); };
        registry["random_delay"] = [this]() {random_delay();};

        for (auto& i : j.at("chaos").items()){
            if(registry.count(i.key()) < 1){
                registry.erase(i.key());
            }
        }
}

void ChaosEngine::dispatch(){
    if(!j.at("chaos").at("enabled")){return;}
    for(auto& i : registry){
        registry[i.first]();
    }
}