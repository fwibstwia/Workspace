#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;;

int computeDist(string a, string b){
  int dist = 0;
  for(int i = 0; i < a.size(); i ++){
    if(a[i] != b[i]){
      dist ++;
    }
      
    if(dist >= 2){
      return dist;
    }
  }
  return dist;
}

  
void printpath(vector<vector<string> > &result, vector<string> &words, vector<vector<int> > &pi, vector<int> &path){
  if(path[path.size() - 1] == 0){
    vector<string> t;
    for (int i = path.size() - 1; i > -1; i--) {
      t.push_back(words[path[i] ]);
    }
    result.push_back(t);
  }else{
    vector<int> pis = pi[path[path.size() - 1] ];
    for(auto v : pis){
      path.push_back(v);
      printpath(result, words, pi, path);
      path.pop_back();	
    }
  }
}

vector<vector<string> > findLadders(string beginWord, string endWord, unordered_set<string> &wordList) {
  vector<string> words;
  words.push_back(beginWord);
  for(auto s: wordList){
    words.push_back(s);
  }

  words.push_back(endWord);
  vector<vector<int > > adj;
  adj.resize(words.size());


  vector<int> color(words.size(), 0);
  vector<int> d(words.size(), 0);
  vector<vector<int> > pi(words.size());
  
  vector<vector<string> > result;
  if(computeDist(beginWord, endWord) == 0){
    vector<string> t;
    t.push_back(beginWord);
    result.push_back(t);
    return result;
  }

	

	
  for(int i = 0; i < words.size(); i ++){
    for(int j = i + 1; j < words.size(); j ++){
      if(computeDist(words[i], words[j]) == 1){
	adj[i].push_back(j);
	adj[j].push_back(i);
      }
    }
  }
	

	
  vector<int> q;

  color[0] = 1;
  d[0] = 0;


  q.push_back(0);
  while(q.size() != 0){
    int u = q[0];
    q.erase(q.begin());
    for(auto v : adj[u]){
      if(color[v] == 0){
	color[v] = 1;
	d[v] = d[u] + 1;
	pi[v].push_back(u);
	q.push_back(v);
      }else if (color[v] == 1){
	if(d[u] + 1 == d[v]){
	  pi[v].push_back(u);
	}
      }
    }
    color[u] = 2;
  }



  if(d[words.size() - 1] == 0){
    return result;
  }

  vector<int> path;
  path.push_back(words.size() - 1);
	
  printpath(result, words, pi, path);

  return result;	
}

 


int main(){

  unordered_set<string> dict({"hot","dot","dog","lot","log"});

  vector<vector< string> > r = findLadders("hit", "cog", dict);
  for (auto i = r.begin(); i != r.end(); ++i){
    for(auto j = (*i).begin(); j != (*i).end(); j ++){
       cout << *j << ' ';
    }
    cout << endl;
  }
  //cout << r << endl;
}
