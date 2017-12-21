#include<bits/stdc++.h>
using namespace std;

class AiHw2{

    private:
        typedef pair<int,int> myPair;
        typedef unordered_map<int,int> myMap;
        typedef vector<pair<pair<int,int>,pair<char,int>>> myList;
        typedef vector<char> myVect;
        typedef pair<pair<int,int>,int> myVal;

        unordered_map<size_t, myList> lookUpTableForList;
        unordered_map<size_t, pair<string,int>> lookUpTableForBFS;
        unordered_map<size_t, string> lookUpTableForGravity;

        hash<string> hashFn;
        myMap colMap;
        int initialize=0;
        int cutoff=3;

    public:
        string fruits;
        int n,p;
        float remTime;
        float beginTime; 

        void display(string arr, int xx, int yy){
            ofstream fout("output.txt");
            fout<<(char)(65+yy)<<xx+1<<endl;
            for(int i=0;i<n*n;i++){
                fout<<arr[i];
                if((i+1)%n==0)
                    fout<<endl;
            }
        }
        
        static bool sortByScore(const pair<pair<int,int>,pair<char,int>> &a,
                const pair<pair<int,int>,pair<char,int>> &b){
            return (a.second.second>b.second.second);
        }        

        size_t calculateHash(string arr){
            return hashFn(arr);
        }

        void applyGravity(string &arr, int x, int y){

            string temp = arr;
            temp.append(to_string(x));
            temp.append(to_string(y));

            size_t hashVal = calculateHash(temp);

            if( (lookUpTableForGravity.find(hashVal) != lookUpTableForGravity.end()) ){
                arr = lookUpTableForGravity[hashVal];
            }
            
            myMap::iterator it;
            myVect fruitsDropped;
            myVect emptySpaces;
            myVect::iterator iter;

            for(it=colMap.begin();it!=colMap.end();it++){
                fruitsDropped.clear();
                emptySpaces.clear();
                int i;
                for(i=0;i<=(*it).second;i++){
                    if( arr[(i*n)+(*it).first] == '*')
                        emptySpaces.push_back(arr[(i*n)+(*it).first]);
                    else
                        fruitsDropped.push_back(arr[(i*n)+(*it).first]);
                }

                i=-1;
                for(iter=emptySpaces.begin();iter!=emptySpaces.end();iter++)
                    arr[(++i*n)+(*it).first] = (*iter);

                for(iter=fruitsDropped.begin();iter!=fruitsDropped.end();iter++)
                    arr[(++i*n)+(*it).first] = (*iter);
            }
            lookUpTableForGravity[hashVal] = arr;
        }

        int makeOneMove(string &arr, int x, int y, char fruit){

            string temp = arr;
            temp.append(to_string(x));
            temp.append(to_string(y));

            colMap.clear();

            size_t hashVal = calculateHash(temp);

            if( (lookUpTableForBFS.find(hashVal) != lookUpTableForBFS.end()) ){
                arr = lookUpTableForBFS[hashVal].first;
                return lookUpTableForBFS[hashVal].second;
            }

            queue<myPair> qu;

            qu.push(make_pair(x,y));
            int count = 0;
            arr[(x*n)+y] = '*';

            while(!qu.empty()){
                myPair co_ord = qu.front();
                qu.pop();

                int i = co_ord.first;
                int j = co_ord.second;
                count++;
                
                if( colMap.find(j) == colMap.end() )
                    colMap[j] = i;
              
                else
                    colMap[j] = max(colMap[j],i);

                if( j+1<n && arr[(i*n)+(j+1)]==fruit ){
                    qu.push(make_pair(i,j+1));
                    arr[(i*n)+(j+1)]='*';
                }

                if( i-1>=0 && arr[((i-1)*n)+j]==fruit ){
                    qu.push(make_pair(i-1,j));
                    arr[((i-1)*n)+j]='*';
                }

                if( i+1<n && arr[((i+1)*n)+j]==fruit ){
                    qu.push(make_pair(i+1,j));
                    arr[((i+1)*n)+j]='*';
                }

                if( j-1>=0 && arr[(i*n)+(j-1)]==fruit ){
                    qu.push(make_pair(i,j-1));
                    arr[(i*n)+(j-1)]='*';
                }
            }

            lookUpTableForBFS[hashVal] = make_pair(arr, count);

            return count;
        }


        bool isGameOver(string arr){
            for(int i=0;i<n;i++)
                for(int j=0;j<n;j++)
                    if(arr[(i*n)+j]!='*')
                        return false;
            return true;
        }

        myVal minimaxAlphaBeta(string arr, int depth, bool isMax, int alpha, int beta, int score){

            string temp;
            temp=arr;

            myList fruitsGroup;
            size_t hashVal = calculateHash(temp);

            //find initial group of fruits
            if(lookUpTableForList.find(hashVal)!=lookUpTableForList.end()){
                fruitsGroup = lookUpTableForList[hashVal];
            } else{
                for(int i=0;i<n;i++){
                    for(int j=0;j<n;j++){
                        if( temp[(i*n)+j]!='*' ){
                            char ch = temp[(i*n)+j];
                            int cost = makeOneMove(temp, i, j, temp[(i*n)+j]);
                            fruitsGroup.push_back( make_pair( make_pair(i,j), make_pair(ch,cost) ) );
                        }
                    }
                }
                if(!initialize){
                    int totalIslands = fruitsGroup.size();
                    float currTime = (float)time(NULL)-beginTime;
                    float actualTime = remTime-currTime;
                    
                    if(actualTime<=0.003){
                        cutoff=1;
                    }
                    else if(actualTime<=0.05){
                        if(totalIslands<=10)
                            cutoff=2;
                        else
                            cutoff=1;
                    }

                    else if(actualTime <= 0.25){
                        if(totalIslands>300)
                            cutoff=1;
                        else if(totalIslands>100)
                            cutoff=2;
                        else
                            cutoff=3;
                    }
                    
                    else if(totalIslands>300){
                        if(actualTime<15)
                            cutoff=1;
                    }

                    else if(totalIslands<=75 && totalIslands>35){
                        if(actualTime<40)
                            cutoff=3;
                        else
                            cutoff=5;
                    }

                    else if(totalIslands<=35 && totalIslands>15){
                        if(actualTime<60)
                            cutoff=5;
                        else
                            cutoff=7;
                    }

                    else if(totalIslands<=15){
                        if(actualTime<50)
                            cutoff=7;
                        else
                            cutoff=9;
                    }
                    initialize=1;
                }
                lookUpTableForList[hashVal]=fruitsGroup;
            }

            sort(fruitsGroup.begin(), fruitsGroup.end(), sortByScore);
            if(isMax){
                myVal best = make_pair(make_pair(-1,-1),INT_MIN);

                myList::iterator it;
                for(it=fruitsGroup.begin();it!=fruitsGroup.end();it++){
                    string fruits_copy;
                    fruits_copy = arr;

                    int cost = makeOneMove(fruits_copy, (*it).first.first, (*it).first.second, (*it).second.first);

                    if(isGameOver(fruits_copy) || depth+1==cutoff){
                        return make_pair(make_pair((*it).first.first, (*it).first.second),score+cost*cost);
                    }

                    applyGravity(fruits_copy,(*it).first.first,(*it).first.second);

                    myVal val = minimaxAlphaBeta(fruits_copy, depth+1, false, alpha, beta, score+cost*cost);

                    if(best.second < val.second){
                        best = val;
                        if(depth==0){
                            best.first.first = (*it).first.first;
                            best.first.second = (*it).first.second;
                        }
                    }

                    alpha = max(alpha, best.second);

                    if(beta<=alpha){
                        break;
                    }
                }
                return best;
            } else {
                myVal best = make_pair(make_pair(-1,-1),INT_MAX);

                myList::iterator it;
                for(it=fruitsGroup.begin();it!=fruitsGroup.end();it++){
                    string fruits_copy;
                    fruits_copy = arr;

                    int cost = makeOneMove(fruits_copy, (*it).first.first, (*it).first.second, (*it).second.first);

                    if(isGameOver(fruits_copy) || depth+1==cutoff){
                        return make_pair(make_pair((*it).first.first, (*it).first.second),score-cost*cost);
                    }

                    applyGravity(fruits_copy,(*it).first.first,(*it).first.second);

                    myVal val = minimaxAlphaBeta(fruits_copy, depth+1, true, alpha, beta, score-cost*cost);

                    if(best.second > val.second)
                        best=val;

                    beta = min(beta, best.second);

                    if(beta<=alpha){
                        break;
                    }
                }
                return best;
            }
        }

        void start(){

            myVal moveCost = minimaxAlphaBeta(fruits, 0, true, INT_MIN, INT_MAX, 0);
            int xx = moveCost.first.first;
            int yy = moveCost.first.second;
            makeOneMove(fruits, xx, yy, fruits[(xx*n)+yy]);
            
            if(cutoff==1){

                myMap::iterator it;
                myVect fruitsDropped;
                myVect emptySpaces;
                myVect::iterator iter;

                for(int i=0;i<n;i++){
                    fruitsDropped.clear();
                    emptySpaces.clear();

                    for(int j=0;j<n;j++){
                        if( fruits[(j*n)+i] == '*')
                            emptySpaces.push_back(fruits[(j*n)+i]);
                        else
                            fruitsDropped.push_back(fruits[(j*n)+i]);
                    }

                    int k=-1;
                    for(iter=emptySpaces.begin();iter!=emptySpaces.end();iter++)
                        fruits[(++k*n)+i] = (*iter);

                    for(iter=fruitsDropped.begin();iter!=fruitsDropped.end();iter++)
                        fruits[(++k*n)+i] = (*iter);

                } 
            }
            else{
                applyGravity(fruits,xx,yy);
            }

            display(fruits, xx, yy);
        }
};

int main(){
    AiHw2 obj;
    obj.beginTime = time(NULL);
    ifstream fin("input.txt");

    string str;

    getline(fin,str);
    stringstream str1(str);
    str1 >> obj.n;

    getline(fin, str);
    stringstream str2(str);
    str2 >> obj.p;

    getline(fin, str);
    stringstream str3(str);
    str3 >> obj.remTime;

    for(int i=0;i<obj.n;i++){
        getline(fin, str);
        str = str.substr(0,obj.n);
        obj.fruits.append(str);
    }
    
    obj.start();
    return 0;
}
