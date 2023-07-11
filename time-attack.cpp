#include <iostream>
#include <inttypes.h>
#include <cstring>
#include <unordered_map>
#include <list> 
#include <vector>
#include <math.h>
#include <chrono>
#include <algorithm>
#include <random>

static __inline__ uint64_t rdtsc()
{
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)lo) | (((uint64_t)hi) << 32);
}

bool check_password (const char * pwd)
{
    const char * c = "thisisavalidpasword";
    while (*pwd == *c && *pwd != '\0' && *c != '\0')
    {
        ++pwd; ++c;
    }
    return *pwd == *c && *pwd == '\0';
}

std::unordered_map<char, double> calculate_mean(std::unordered_map<char, int> sum, int N){
    std::unordered_map<char, double> meanMap;
    for(auto letter: sum){
        meanMap[letter.first]= letter.second/N;
    }
    return meanMap;
}

std::unordered_map<char, double> calculate_variance(std::unordered_map<char, double> meanMap, int N, std::unordered_map<char, int> sumSquares) {
    std::unordered_map<char, double> varianceMap;
    for(auto letter: sumSquares) {
        varianceMap[letter.first] = (1.10/(N-1.0)) * (letter.second - (N * meanMap[letter.first]));
    }
    return varianceMap;
    
}

char compute_confidence_intervals(std::unordered_map<char, double> meanMap, std::unordered_map<char, double> varianceMap, double Z, int N) {
    char maxLetter = '\0';
    double maxLower = -1;
    double maxUpper = -1;

    for(auto letter: meanMap){
        double std_deviation = sqrt(varianceMap[letter.first]);
        double lowBound = letter.second - Z*std_deviation/N;
        double upBound = letter.second + Z*std_deviation/N;

        // std::cout<< "\'" <<letter.first << "\':"<< "(" << lowBound << "," << upBound << ")" << std::endl;
        if(lowBound > maxLower) {
            maxLower = lowBound;
            maxLetter = letter.first;
        }else if(upBound > maxUpper) {
            maxUpper = upBound;
        }
    }

    return maxLower > maxUpper ? maxLetter : '\0';
}

void warmup(int warmUpCycles) {
    char* guess = "thisisaguess";
    for(int i =0; i< warmUpCycles;i++){
        bool ans = check_password(guess);
    }
}



int main() {
    int N = 1000;
    double Z = 1.960;
    int WARMUP_CYCLES = 10000;

    std::vector<char> letterList{
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z'
    };


    std::string currentGuess = "";
    std::unordered_map<char, int> sum;
    std::unordered_map<char, int> sumSquares;

    while(true) {
        for(int i = 0; i<26; i++) {
            sum[char('a'+i)] = 0;
            sumSquares[char('a'+i)] = 0;
        }

        
        warmup(WARMUP_CYCLES);
        
        for(int i = 0; i<N; i++){
            
            // std::list<uint64_t> runtimes;
            std::random_shuffle(letterList.begin(), letterList.end());

            for(auto letter: letterList) {

                size_t originalLength = currentGuess.length();
                char* guess = new char[originalLength + 2];
                strcpy(guess, currentGuess.c_str());
                guess[originalLength] = letter;  
                guess[originalLength + 1] = '\0';
        
                
                uint64_t start = rdtsc();
                bool ans = check_password(guess);
                uint64_t difference = rdtsc() - start;
                
                if(ans == true) {
                    std::cout<<"PASSWORD CRACKED: " << guess << std::endl;
                    return 0;
                }
                
                sum[letter] += (difference);            
                sumSquares[letter] += (difference*difference);

                
                // std::cout<<difference<<" "<<guess<<" "<<sumSquares['p']<<std::endl;
            }

        }

        std::unordered_map<char, double> meanMap = calculate_mean(sum, N);
        std::unordered_map<char, double> varianceMap = calculate_variance(meanMap, N, sumSquares);
        char letter = compute_confidence_intervals(meanMap, varianceMap, Z, N);
        if( letter != '/0') {
            currentGuess += letter;
        }
        std::cout<< currentGuess << std::endl;
    }   

    return 0;
}