#define DRIVER_VERSION "0"

#include<iostream>
#include<fstream>
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

class MasterSiteQuadrupartitionOneHotScorer{
public:
    typedef bool FreqType;
    typedef double EqFreqType;
    typedef double ScoreType;
    typedef bool CounterType;

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

MasterSiteQuadrupartitionOneHotScorer::DataType parseSubstring(const string &s1, const string &s2, const string &s3, const string &s4, int start, int end){
    MasterSiteQuadrupartitionOneHotScorer::DataType res;
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

int main(int argc, char *argv[])
{
    int windowSize = 1000;
    int intervalSize = 10000;
    
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
        MasterSiteQuadrupartitionOneHotScorer::DataType data = parseSubstring(s1, s2, s3, s4, pos, pos + intervalSize);
        vector<double> topology1 = MasterSiteQuadrupartitionOneHotScorer::slidingWindow(windowSize, data.cnt0, data.cnt3, data.cnt1, data.cnt2, data.pi);
        vector<double> topology2 = MasterSiteQuadrupartitionOneHotScorer::slidingWindow(windowSize, data.cnt1, data.cnt3, data.cnt0, data.cnt2, data.pi);
        vector<double> topology3 = MasterSiteQuadrupartitionOneHotScorer::slidingWindow(windowSize, data.cnt2, data.cnt3, data.cnt0, data.cnt1, data.pi);
        for (int i = 0; i < topology1.size(); i++){
            double total = topology1[i] + topology2[i] + topology3[i];
            fout << pos + i * windowSize << "\t" << topology1[i] << "\t" << topology2[i] << "\t" << topology3[i] << "\t" <<\
                topology1[i] / total << "\t" << topology2[i] / total << "\t" << topology3[i] / total << endl;
        }
    }
    return 0;
}