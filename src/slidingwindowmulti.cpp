#define DRIVER_VERSION "0"

#include<iostream>
#include<fstream>
#include<sstream>
#include<unordered_map>
#include<unordered_set>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<random>
#include<thread>
#include<mutex>

using namespace std;

template<typename FreqType = unsigned short, typename EqFreqType = double, typename ScoreType = double, typename CounterType = long long> class MasterSiteQuadrupartitionScorer{
public:
    struct DataType{
        array<vector<FreqType>, 4> cnt0, cnt1, cnt2, cnt3;
        array<EqFreqType, 4> pi;
    };
    
private:
    inline static CounterType quadXXYY(CounterType x0, CounterType x1, CounterType x2, CounterType x3, CounterType y0, CounterType y1, CounterType y2, CounterType y3){
	    return x0 * x1 * y2 * y3 + y0 * y1 * x2 * x3;
    }

    static ScoreType scoreSite(int pos, const array<vector<FreqType>, 4> &cnt0, const array<vector<FreqType>, 4> &cnt1,
            const array<vector<FreqType>, 4> &cnt2, const array<vector<FreqType>, 4> &cnt3, const array<EqFreqType, 4> &pi){
        const EqFreqType A = pi[0], C = pi[1], G = pi[2], T = pi[3];
        const EqFreqType R = A + G, Y = C + T, R2 = A * A + G * G, Y2 = C * C + T * T;
        const FreqType a0 = cnt0[0][pos], c0 = cnt0[1][pos], g0 = cnt0[2][pos], t0 = cnt0[3][pos], r0 = a0 + g0, y0 = c0 + t0;
        const FreqType a1 = cnt1[0][pos], c1 = cnt1[1][pos], g1 = cnt1[2][pos], t1 = cnt1[3][pos], r1 = a1 + g1, y1 = c1 + t1;
        const FreqType a2 = cnt2[0][pos], c2 = cnt2[1][pos], g2 = cnt2[2][pos], t2 = cnt2[3][pos], r2 = a2 + g2, y2 = c2 + t2;
        const FreqType a3 = cnt3[0][pos], c3 = cnt3[1][pos], g3 = cnt3[2][pos], t3 = cnt3[3][pos], r3 = a3 + g3, y3 = c3 + t3;

        const CounterType rryy = quadXXYY(r0, r1, r2, r3, y0, y1, y2, y3);

        const CounterType aayy = quadXXYY(a0, a1, a2, a3, y0, y1, y2, y3);
        const CounterType ggyy = quadXXYY(g0, g1, g2, g3, y0, y1, y2, y3);
        const CounterType rrcc = quadXXYY(r0, r1, r2, r3, c0, c1, c2, c3);
        const CounterType rrtt = quadXXYY(r0, r1, r2, r3, t0, t1, t2, t3);
        
        const CounterType aacc = quadXXYY(a0, a1, a2, a3, c0, c1, c2, c3);
        const CounterType aatt = quadXXYY(a0, a1, a2, a3, t0, t1, t2, t3);
        const CounterType ggcc = quadXXYY(g0, g1, g2, g3, c0, c1, c2, c3);
        const CounterType ggtt = quadXXYY(g0, g1, g2, g3, t0, t1, t2, t3);
        
        return rryy * R2 * Y2 - (aayy + ggyy) * (R * R) * Y2 - (rrcc + rrtt) * R2 * (Y * Y)
            + (aacc + aatt + ggcc + ggtt) * (R * R) * (Y * Y);
    }

public:
    static ScoreType scoreInterval(int start, int end, const array<vector<FreqType>, 4> &cnt0, const array<vector<FreqType>, 4> &cnt1,
            const array<vector<FreqType>, 4> &cnt2, const array<vector<FreqType>, 4> &cnt3, const array<EqFreqType, 4> &pi){
        ScoreType res = 0;
        for (int i = start; i < end; i++) res += scoreSite(i, cnt0, cnt1, cnt2, cnt3, pi);
        return res;
    }

    static ScoreType score(const array<vector<FreqType>, 4> &cnt0, const array<vector<FreqType>, 4> &cnt1,
            const array<vector<FreqType>, 4> &cnt2, const array<vector<FreqType>, 4> &cnt3, const array<EqFreqType, 4> &pi){
        return scoreInterval(0, cnt0[0].size(), cnt0, cnt1, cnt2, cnt3, pi);
    }

    static vector<ScoreType> slidingWindow(int windowSize, const array<vector<FreqType>, 4> &cnt0, const array<vector<FreqType>, 4> &cnt1,
            const array<vector<FreqType>, 4> &cnt2, const array<vector<FreqType>, 4> &cnt3, const array<EqFreqType, 4> &pi){
        vector<ScoreType> res;
        for (int i = 0; i + windowSize <= cnt0[0].size(); i += windowSize){
            res.push_back(scoreInterval(i, i + windowSize, cnt0, cnt1, cnt2, cnt3, pi));
        }
        return res;
    }
};

typedef MasterSiteQuadrupartitionScorer<bool, double, double, bool> MasterSiteQuartetOneHotScorer;
typedef MasterSiteQuadrupartitionScorer<unsigned char, double, double, int> MasterSiteSmallCountQuadrupartitionScorer;
typedef MasterSiteQuadrupartitionScorer<unsigned short, double, double, long long> MasterSiteNormalQuadrupartitionScorer;

MasterSiteQuartetOneHotScorer::DataType parseSubstring(const string &s1, const string &s2, const string &s3, const string &s4, int start, int end){
    MasterSiteQuartetOneHotScorer::DataType res;
    array<const string*, 4> lst = {&s1, &s2, &s3, &s4};
    array<array<vector<bool>, 4>*, 4> cntlst = {&res.cnt0, &res.cnt1, &res.cnt2, &res.cnt3};
    for (int i = 0; i < 4; i++){
        const string &s = *(lst[i]);
        array<vector<bool>, 4> &cnt = *(cntlst[i]);
        for (int j = start; j < end; j++){
            cnt[0].push_back(s[j] == 'A' || s[j] == 'a');
            cnt[1].push_back(s[j] == 'C' || s[j] == 'c');
            cnt[2].push_back(s[j] == 'G' || s[j] == 'g');
            cnt[3].push_back(s[j] == 'T' || s[j] == 't');
            for (int k = 0; k < 4; k++) res.pi[k] += cnt[k].back();
        }
    }
    double sum = res.pi[0] + res.pi[1] + res.pi[2] + res.pi[3];
    for (int k = 0; k < 4; k++) res.pi[k] /= sum;
    return res;
}

MasterSiteSmallCountQuadrupartitionScorer::DataType parseFreqs(const array<vector<unsigned char>, 4> &f1, const array<vector<unsigned char>, 4> &f2, 
        const array<vector<unsigned char>, 4> &f3, const array<vector<unsigned char>, 4> &f4, int start, int end){
    MasterSiteSmallCountQuadrupartitionScorer::DataType res;
    array<const array<vector<unsigned char>, 4>*, 4> lst = {&f1, &f2, &f3, &f4};
    array<array<vector<unsigned char>, 4>*, 4> cntlst = {&res.cnt0, &res.cnt1, &res.cnt2, &res.cnt3};
    for (int i = 0; i < 4; i++){
        const array<vector<unsigned char>, 4> &f = *(lst[i]);
        array<vector<unsigned char>, 4> &cnt = *(cntlst[i]);
        for (int k = 0; k < 4; k++) {
            for (int j = start; j < end; j++){
                cnt[k].push_back(f[k][j]);
                res.pi[k] += f[k][j];
            }
        }
    }
    double sum = res.pi[0] + res.pi[1] + res.pi[2] + res.pi[3];
    for (int k = 0; k < 4; k++) res.pi[k] = (sum == 0) ? 0 : res.pi[k] / sum;
    return res;
}

MasterSiteNormalQuadrupartitionScorer::DataType parseFreqs(const array<vector<unsigned short>, 4> &f1, const array<vector<unsigned short>, 4> &f2, 
        const array<vector<unsigned short>, 4> &f3, const array<vector<unsigned short>, 4> &f4, int start, int end){
    MasterSiteNormalQuadrupartitionScorer::DataType res;
    array<const array<vector<unsigned short>, 4>*, 4> lst = {&f1, &f2, &f3, &f4};
    array<array<vector<unsigned short>, 4>*, 4> cntlst = {&res.cnt0, &res.cnt1, &res.cnt2, &res.cnt3};
    for (int i = 0; i < 4; i++){
        const array<vector<unsigned short>, 4> &f = *(lst[i]);
        array<vector<unsigned short>, 4> &cnt = *(cntlst[i]);
        for (int k = 0; k < 4; k++) {
            for (int j = start; j < end; j++){
                cnt[k].push_back(f[k][j]);
                res.pi[k] += f[k][j];
            }
        }
    }
    double sum = res.pi[0] + res.pi[1] + res.pi[2] + res.pi[3];
    for (int k = 0; k < 4; k++) res.pi[k] = (sum == 0) ? 0 : res.pi[k] / sum;
    return res;
}

int onehot(int argc, char *argv[])
{
    int windowSize = 10;
    int intervalSize = 20;
    
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    int n, L;
    string name1, name2, name3, name4;
    string s1, s2, s3, s4;
    fin >> n >> L;
    fin >> name1 >> s1;
    fin >> name2 >> s2;
    fin >> name3 >> s3;
    fin >> name4 >> s4;
    fout << "pos" << "\t" << name1 << "\t" << name2 << "\t" << name3 << "\t" << name1 << "\t" << name2 << "\t" << name3 << endl;
    for (int pos = 0; pos + intervalSize <= L; pos += intervalSize){
        MasterSiteQuartetOneHotScorer::DataType data = parseSubstring(s1, s2, s3, s4, pos, pos + intervalSize);
        vector<double> topology1 = MasterSiteQuartetOneHotScorer::slidingWindow(windowSize, data.cnt0, data.cnt3, data.cnt1, data.cnt2, data.pi);
        vector<double> topology2 = MasterSiteQuartetOneHotScorer::slidingWindow(windowSize, data.cnt1, data.cnt3, data.cnt0, data.cnt2, data.pi);
        vector<double> topology3 = MasterSiteQuartetOneHotScorer::slidingWindow(windowSize, data.cnt2, data.cnt3, data.cnt0, data.cnt1, data.pi);
        for (int i = 0; i < topology1.size(); i++){
            double total = topology1[i] + topology2[i] + topology3[i];
            fout << pos + i * windowSize << "\t" << topology1[i] << "\t" << topology2[i] << "\t" << topology3[i] << "\t" <<
                topology1[i] / total << "\t" << topology2[i] / total << "\t" << topology3[i] / total << endl;
        }
    }
    return 0;
}

template<typename FreqType = unsigned short, typename Scorer = MasterSiteNormalQuadrupartitionScorer>
    string multiind(string input, string mapping = "", int intervalSize = 1000000, int windowSize = 10000, bool header = true)
{
    string name[4];
    unordered_map<string, int> name2id;
    unordered_map<string, int> realname2id;
    if (mapping != ""){
        ifstream fmap(mapping);
        string idname, realname;
        while(fmap >> idname){
            fmap >> realname;
            if (!realname2id.count(realname)) {
                name[realname2id.size()] = realname;
                realname2id[realname] = realname2id.size();
            }
            name2id[idname] = realname2id[realname];
        }
    }
    ifstream fin(input);
    ostringstream fout;
    string line;
    int id, pos;
    array<array<vector<FreqType>, 4>, 4> freq;
    while (getline(fin, line)){
        if (line[0] == '>'){
            if (!name2id.count(line.substr(1))) {
                name[realname2id.size()] = line.substr(1);
                realname2id[line.substr(1)] = realname2id.size();
                name2id[line.substr(1)] = realname2id[line.substr(1)];
            }
            id = name2id[line.substr(1)];
            pos = 0;
        }
        else{
            for (int j = 0; j < line.size(); j++){
                for (int k = 0; k < 4; k++){
                    if (pos + j >= freq[id][k].size()) freq[id][k].push_back(0); 
                }
                freq[id][0][pos + j] += (line[j] == 'A' || line[j] == 'a');
                freq[id][1][pos + j] += (line[j] == 'C' || line[j] == 'c');
                freq[id][2][pos + j] += (line[j] == 'G' || line[j] == 'g');
                freq[id][3][pos + j] += (line[j] == 'T' || line[j] == 't'); 
            }
            pos += line.size();
        }
    }
    if (header) fout << "pos" << "\t" << name[0] << "\t" << name[1] << "\t" << name[2] << endl;
    for (int pos = 0; pos + intervalSize <= freq[0][0].size(); pos += intervalSize){
        typename Scorer::DataType data = parseFreqs(freq[0], freq[1], freq[2], freq[3], pos, pos + intervalSize);
        vector<double> topology1 = Scorer::slidingWindow(windowSize, data.cnt0, data.cnt3, data.cnt1, data.cnt2, data.pi);
        vector<double> topology2 = Scorer::slidingWindow(windowSize, data.cnt1, data.cnt3, data.cnt0, data.cnt2, data.pi);
        vector<double> topology3 = Scorer::slidingWindow(windowSize, data.cnt2, data.cnt3, data.cnt0, data.cnt1, data.pi);
        double total1 = 0, total2 = 0, total3 = 0;
        for (int i = 0; i < topology1.size(); i++){
            total1 += topology1[i];
            total2 += topology2[i];
            total3 += topology3[i];
        }
        fout << pos << "\t" << total1 << "\t" << total2 << "\t" << total3 << endl;
    }
    return fout.str();
}

int main(int argc, char *argv[])
{
    string s = multiind<>(argv[1], argv[2], 1000, 1000, false);
    cout << argv[1] << "\t" << s;
    return 0;
}