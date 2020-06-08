#include <iostream>
#include <vector>
#include <deque>
#define n 3
using namespace std;

int mat[n][n];
int score_pl1=0,score_pl2=0;
deque <pair<int,int> > pl1,pl2;
//game_state deve solamente ricordare il pezzo e la mossa fatta nella configurazione
//della matrice
struct x{
	int board[n][n];
	pair<pair<int,int>,char> move;
};
vector <x> game_state;
//lily_state deve memorizzare per ogni nuova configurazione della matrice 
// tutte le mosse possibili eliminandone alcune durante la fase di gioco
// cosi' attuando il machine learning
struct y{
	int board[n][n];
	deque<pair<pair<int,int>,char> > all_move;
};
vector <y> lily_state;

//stampa la matrice
void stampa(){
	cout<<"ij ";
	for(int i=0;i<n;i++)
		cout<<i<<' ';
	cout<<endl<<endl;
	for(int i=0;i<n;i++){
		cout<<i<<"  ";
		for(int j=0;j<n;j++){
			cout<<mat[i][j]<<' ';
		}
		cout<<endl;
	}
	cout<<endl;
	cout<<"Player1 "<<score_pl1<<"   Player2 "<<score_pl2<<endl;
}

int know_matrix(){
	int c;
	for(int k=0;k<lily_state.size();k++){
		c=0;
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				if(mat[i][j]==lily_state[k].board[i][j])
					c++;
		if(c==n*n)
			return k;
	}
	return -1;
}

//vede se la mossa e' legale
pair <int,int> can_move(int i,int j,char direction,int player){
	pair<int,int> position={-1,-1};
	int a=i,b=j;
	//se il player e' 1 i pezzi devono decrementare la i
	//se il player e' 2 i pezzi devono incrementare la i
	if(direction=='w'){
		if(player==2)
			return position;
		if(mat[i-1][j]!=0)
			return position;
		a--;
	}
	else if(direction=='a'){
		a=(player==1)?i-1:i+1;
		b--;
		if(mat[a][b]==player || mat[a][b]==0)
			return position;
	}	
	else if(direction=='s'){
		if(player==1)
			return position;
		if(mat[i+1][j]!=0)
			return position;
		a++;
	}
	else if(direction=='d'){
		a=(player==1)?i-1:i+1;;
		b++;
		if(mat[a][b]==player || mat[a][b]==0)
			return position;
	}
	if(i>=n || j>=n || a>=n || b>=n)
		return position;
	if(i<0 || j<0 || a<0 || b<0)
		return position;
	if(mat[i][j]!=player)
		return position;
	if(mat[a][b]==player)
		return position;
		
	position.first=a;
	position.second=b;
	return position;
}

//ritorna 1 se vince player1, 2 se vince player2,
//0 se la partita puo continuare
int win(int player){
	for(int j=0;j<n;j++)
		if(mat[0][j]==1)
			return 1;
	for(int j=0;j<n;j++)
		if(mat[n-1][j]==2)
			return 2;
	if(pl1.size()>0 && pl2.size()==0)
		return 1;
	if(pl2.size()>0 && pl1.size()==0)
		return 2;
	if(player==1){
		for(int x=0;x<pl2.size();x++){
			if(can_move(pl2[x].first,pl2[x].second,'w',2).first!=-1)
				return 0;
			if(can_move(pl2[x].first,pl2[x].second,'a',2).first!=-1)
				return 0;
			if(can_move(pl2[x].first,pl2[x].second,'s',2).first!=-1)
				return 0;
			if(can_move(pl2[x].first,pl2[x].second,'d',2).first!=-1)
				return 0;
		}
	}
	if(player==2){
		for(int x=0;x<pl1.size();x++){
			if(can_move(pl1[x].first,pl1[x].second,'w',1).first!=-1)
				return 0;
			if(can_move(pl1[x].first,pl1[x].second,'a',1).first!=-1)
				return 0;
			if(can_move(pl1[x].first,pl1[x].second,'s',1).first!=-1)
				return 0;
			if(can_move(pl1[x].first,pl1[x].second,'d',1).first!=-1)
				return 0;
		}
	}
	return player;
}

//giocatore normale
void player1(){
	int i,j;
	pair <int,int> cord;
	char direction;
	do{
		cout<<"inserisci le coordinate del pezzo che vuoi muovere"<<endl;
		cin>>i>>j;
		cout<<"inserisci la direzione di dove lo vuoi spostare (w,a,d)"<<endl;
		cout<<"w = avanti"<<endl<<"a = mangia a sinistra"<<endl<<"d = mangia a destra"<<endl;
		cin>>direction;
		cord=can_move(i,j,direction,1);
		if(cord.first==-1){
			cout<<"inserisci correttamente i dati"<<endl;
			_sleep(1000);
			system("CLS");
			stampa();
		}
	}while(cord.first==-1);
	mat[i][j]=0;
	for(int k=0;k<pl1.size();k++){
		if(pl1[k].first==i && pl1[k].second==j){
			pl1[k].first=cord.first;
			pl1[k].second=cord.second;
			break;
		}
	}
	if(mat[cord.first][cord.second]==2){
		for(int i=0;i<pl2.size();i++)
			if(pl2[i].first==cord.first && pl2[i].second==cord.second){
				pl2.erase(pl2.begin()+i);
				break;
			}
	}
	mat[cord.first][cord.second]=1;
}

//Lily
void player2(){
	int index=know_matrix();
	if(index==-1){
		y new_status;
		//inserisco tutte le combinazioni possibili
		for(int k=0;k<pl2.size();k++){
			new_status.all_move.push_back({{pl2[k].first,pl2[k].second},'w'});
			new_status.all_move.push_back({{pl2[k].first,pl2[k].second},'a'});
			new_status.all_move.push_back({{pl2[k].first,pl2[k].second},'s'});
			new_status.all_move.push_back({{pl2[k].first,pl2[k].second},'d'});
		}
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				new_status.board[i][j]=mat[i][j];
		//inserisci tutto in lily_state()
		lily_state.push_back(new_status);
		index=lily_state.size()-1;
	}
	//scgle un pezzo da muovere
	unsigned int i,j,f,g;
	pair <int,int> cord;
	do{
		i=rand()%lily_state[index].all_move.size();
		cord=can_move(lily_state[index].all_move[i].first.first,
		lily_state[index].all_move[i].first.second,
		lily_state[index].all_move[i].second,2);
		if(cord.first==-1)
			lily_state[index].all_move.erase(lily_state[index].all_move.begin()+i);
	}while(cord.first==-1);
	f=lily_state[index].all_move[i].first.first;
	g=lily_state[index].all_move[i].first.second;
	//se la mossa e' compatibile inseriscila in game_state
	x right_move;
	right_move.move={{f,g},lily_state[index].all_move[i].second};
	for(int a=0;a<n;a++)
		for(int b=0;b<n;b++)
			right_move.board[a][b]=mat[a][b];
	game_state.push_back(right_move);
	
	mat[f][g]=0;
	for(int a=0;a<pl2.size();a++){
		if(pl2[a].first==f && pl2[a].second==g){
			pl2[a].first=cord.first;
			pl2[a].second=cord.second;
			break;
		}
	}
	if(mat[cord.first][cord.second]==1){
		for(int a=0;a<pl1.size();a++)
			if(pl1[a].first==cord.first && pl1[a].second==cord.second){
				pl1.erase(pl1.begin()+a);
				break;
			}
	}
	mat[cord.first][cord.second]=2;
}

int find_matrix(int index){
	int c;
	for(int k=0;k<lily_state.size();k++){
		c=0;
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				if(game_state[index].board[i][j]==lily_state[k].board[i][j])
					c++;
		if(c==n*n)
			return k;
	}
	return -1;
}

void learn(){
	int l=game_state.size()-1;
	int index,c1,c2;
	char ch;
	//do{
	index=find_matrix(l);
	c1=game_state[l].move.first.first;
	c2=game_state[l].move.first.second;
	ch=game_state[l].move.second;
	for(int i=0;i<lily_state[index].all_move.size();i++){
		if(c1==lily_state[index].all_move[i].first.first &&
		   c2==lily_state[index].all_move[i].first.second &&
		   ch==lily_state[index].all_move[i].second){
			lily_state[index].all_move.erase(lily_state[index].all_move.begin()+i);
			break;
		}
	}
}

int main(){
	int t,last_move;
	cout<<"***inserisci il numero di round***"<<endl;
	cin>>t;
	for(int round=0;round<t;round++){
		//per ogni partita game_state va azzerato
		game_state.clear();
		//all'inizio di ogni round la griglia viene inizializzata
		for(int i=0;i<n;i++){
			for(int j=0;j<n;j++){
				if(i==0){
					mat[i][j]=2;
				}
				else if(i==n-1)
					mat[i][j]=1;
				else
					mat[i][j]=0;
			}
		}
		//inizializzo i due giocatori
		for(int j=0;j<n;j++){
			pl1.push_back({n-1,j});
			pl2.push_back({0,j});
		}
		//inizia il gioco
		while(true){
			system("CLS");
			stampa();
			player1();
			last_move=1;
			if(win(last_move)!=0)
				break;
			system("CLS");
			stampa();
			_sleep(1000);
			player2();
			last_move=2;
			if(win(last_move)!=0)
				break;
		}
		if(win(last_move)==1){
			learn();
			score_pl1++;
		}
		else
			score_pl2++;
		//chi ha vinto?
		system("CLS");
		stampa();
		cout<<"Player"<<win(last_move)<<" vince"<<endl;
		_sleep(1500);
	}
}
