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

bool check_password (const char * pwd)
{
    const char * c = "sybigimcpxxkdxjeukujuhrrwjzwfpxirnhvfkgikayzpjcfbhqkrsmtbytjzcfhwcgeeoaslxemlicpdctpkhatfshpsgx";
    while (*pwd == *c && *pwd != '\0' && *c != '\0')
    {
        ++pwd; ++c;
    }
    return *pwd == *c && *pwd == '\0';
}

static __inline__ uint64_t rdtsc()
{
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)lo) | (((uint64_t)hi) << 32);
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

        if(lowBound > maxLower) {
            maxLower = lowBound;
            maxLetter = letter.first;
        }else if(upBound > maxUpper) {
            maxUpper = upBound;
        }
    }

    return maxLower > maxUpper ? maxLetter : '\0';
}

void warmup() {
    int warmUpCycles = 10000;
    for(int i =0; i< warmUpCycles;i++){
        check_password("thisisaguess");
    }
}

char find_letter(std::string currentGuess){
    // double Z = 1.960; // 95%
    double Z = 1.150; // 75%
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::vector<char> letterList{
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z'
    };

    std::unordered_map<char, int> sum;
    std::unordered_map<char, int> sumSquares;

    for(int i = 0; i<26; i++) {
        sum[char('a'+i)] = 0;
        sumSquares[char('a'+i)] = 0;
    }

    long unsigned int count = 0;
    char letter = '\0';
    do{
        count++;
        std::shuffle(letterList.begin(), letterList.end(), e);
        // std::random_shuffle(letterList.begin(), letterList.end());

        for(auto letter: letterList) {
            std::string guess = currentGuess + letter;
    
            uint64_t start = rdtsc();
            check_password(guess.c_str());
            uint64_t difference = rdtsc() - start;
            
            sum[letter] += (difference);            
            sumSquares[letter] += (difference*difference);
        }
        std::unordered_map<char, double> meanMap = calculate_mean(sum, count);
        std::unordered_map<char, double> varianceMap = calculate_variance(meanMap, count, sumSquares);
        letter = compute_confidence_intervals(meanMap, varianceMap, Z, count);
    }while(count < (1000) || letter == '\0');
    std::cout<<count<<std::endl;

    return letter;
}

std::string crack_password() {
    std::string currentGuess = "";

    warmup();

    while(!check_password(currentGuess.c_str())) {
        currentGuess += find_letter(currentGuess);
        std::cout<<currentGuess<<std::endl;
        if(currentGuess.length() %95== 0 ){
            warmup();
        }
    }

    return currentGuess;
}

int main() {
    std::string password = crack_password();
    std::cout<<"PASSWORD CRACKED: " << password << std::endl;

    return 0;
}