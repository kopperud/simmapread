#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

std::vector<std::string> tokenize(std::string &s);
size_t find_split(std::vector<std::string> &s);

int print_subtree(std::vector<std::string> &tokens){
    std::cout << "the subtree: \n";
    size_t n_tokens = tokens.size();

    for (size_t i = 0; i < n_tokens; i++){
        std::cout << tokens[i];
    }
    std::cout << "\n" << std::flush;
    return 0;
}

std::string parse_tiplab(std::string &s){
    size_t idx = s.find(":");
    if (idx == std::string::npos){
        throw std::runtime_error("comma not found, cant parse tip label\n");
    }

    //std::vector<std::string>::const_iterator first = tokens.begin();
    size_t len = s.length();
    std::string res = s.substr(0, idx);
    return res;
}



std::vector<std::string> split_by_colon(std::string &s){
    std::vector<std::string> tokens;
    
    boost::split( tokens, s, boost::is_any_of(":") );
    return tokens;
}


void parse_history(std::string &s){
    size_t curly_idx = s.find("{");

    if (curly_idx == std::string::npos){
        throw std::runtime_error("curly brace not found when parsing branch history\n");
    }

    std::vector<std::string> tokens = split_by_colon(s);

}



/*
function parse_history(s)
    curly_idx = findfirst('{', s)
    h = s[(curly_idx+1):(end-1)]
    states = String[]
    times = Float64[]

    for item in split(h, ':')
        state, time = split(item, ',')
        append!(states, [state])
        append!(times, parse(Float64, time))
    end
    return(states, times)
end
*/

std::vector<std::string> peel_root(std::vector<std::string> &tokens){
    size_t len = tokens.size();

    std::vector<std::string>::const_iterator first = tokens.begin() +1;
    std::vector<std::string>::const_iterator last = tokens.begin() +len-2;

    std::vector<std::string> peeled_tokens(first, last);
    return peeled_tokens;
}

std::vector<std::string> peel(std::vector<std::string> &tokens){
    size_t len = tokens.size();

    std::vector<std::string>::const_iterator first = tokens.begin() +0;
    std::vector<std::string>::const_iterator last = tokens.begin() +len-2;

    std::vector<std::string> peeled_tokens(first, last);
    return peeled_tokens;
}

std::vector<std::vector<std::string>> partition(std::vector<std::string> &tokens){
    size_t comma_idx = find_split(tokens);
    size_t len = tokens.size();

    std::vector<std::string>::const_iterator first = tokens.begin();
    std::vector<std::string>::const_iterator split = tokens.begin() + comma_idx;
    std::vector<std::string>::const_iterator last = tokens.begin() + len;

    std::vector<std::string> left(first, split);
    std::vector<std::string> right(split+1, last);

    std::vector<std::vector<std::string>> res = {left, right};
    return res;
}



int main(){
    std::string s = "((A:{2,0.1:1,0.6},B:{1,0.7}):{1,0.3},C:{2,0.3:1,0.1:2,0.6});"; 

    //std::string* p = &s; 

    std::cout << "the newick string: \n";
    std::cout << s << "\n";

    std::vector<std::string> tokens = tokenize(s);


    std::cout << "the tokens: \n";
    for (size_t i = 0; i < tokens.size(); i++){
        std::cout << tokens[i] << "\n";
    }

    std::cout << "split index: \n";
    std::vector<std::string> peeled = peel_root(tokens);
    print_subtree(peeled);
    size_t split_idx = find_split(peeled);
    std::cout << split_idx << "\n";

    std::vector<std::vector<std::string>> res = partition(peeled);
    std::vector<std::string> left = res[0];
    std::vector<std::string> right = res[1];

    print_subtree(left);
    print_subtree(right);
    //print_subtree(peel(left)); 
    //print_subtree(peel(right)); 
    std::cout << "tip label: \t" << parse_tiplab(right[0]) << "\n";

    std::cout << "making an ostream object: \t" << std::endl;

    std::stringstream stm (std::stringstream::in | std::stringstream::out);
    stm << "Hello there, this is my text";
    char c = ' ';
    stm.get(c);

    //for (size_t i = 0; i < stm.str().length(); i++){
    while( stm.good() ){
        std::cout << c << std::endl;
        stm.get(c);
    }
    return 0;
}

std::vector<std::string> tokenize(std::string &s){
    std::vector<std::string> tokens;
	size_t len = s.length();

	std::string_view v = s;
    std::string token; 
    std::string charsToFind = "(;,)";

    int	i = 0;

	while (i <= len-1){
		if (charsToFind.find(s[i]) != std::string::npos){
			token = std::string(1, s[i]);	
		    tokens.push_back(token);
			i += 1;
		}else{
            std::string_view smap_comment = v.substr(i, len-1);
		    // if is a comment comment	
            bool is_smap_comment = smap_comment.find(":{") != std::string::npos;
    		if (is_smap_comment != std::string::npos){
				size_t close_idx = v.substr(i,len-1).find("}")+1;
				token = v.substr(i, close_idx);
				tokens.push_back(token);
                i += token.length();
		    }else{
				std::vector<size_t> l;
				size_t firstcomma = smap_comment.find(",");
				size_t firstclose = smap_comment.find(")");

                if (firstcomma != std::string::npos){
                    l.push_back(firstcomma+1);
                }
                if (firstclose != std::string::npos){
                    l.push_back(firstclose+1);
                }

                bool is_empty = l.size() == 0;
                if (!is_empty){
                    auto min_element_iterator = std::min_element(l.begin(), l.end());
                    size_t close_idx = *min_element_iterator;
                    token = v.substr(i, close_idx+i+1);
                    tokens.push_back(token);
                    i += token.length();
                }else{
                    i += 1;
                }
		    }
		}
	}
	return tokens;
}

size_t find_split(std::vector<std::string> &tokens){
    size_t ps = 0; //parenthesis counter
    std::string token;

    size_t n_tokens = tokens.size();
    //for (size_t i = 0; i < tokens.size(); i++){
    for (size_t i = 0; i < n_tokens; i++){
       token = tokens[i];

       if (token == "("){
           ps += 1;
       }else if (token == ")"){
           ps -= 1;
       }

       if (token == "," && ps == 0){
           return i;
       }
    }
    throw std::runtime_error("split not found, simmap string deformed\n");
    return 0;
}
