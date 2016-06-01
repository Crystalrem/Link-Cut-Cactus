#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector> 
	class Link_cut_cactus
	{
	public:
		class Node;
		class Edge;
		class Circle;
		class Lcc_message;
		class Path_message;
		class Weight;
		
		class Path_message
		{
		public:
			int mina;
			int minb;
			
			Path_message() {}
			Path_message(const Weight &w)
			{
				mina = w.wa, minb = w.wb;
			}
			Path_message(const int &a, const int &b)
			{
				mina = a, minb = b;
			}
			void setEmpty()
			{
				mina = 0;
				minb = 1000000000;
			}
			void setInvalid()
			{
				mina = -1;
				minb = -1;
			}
			bool valid() const
			{
				return mina != -1;
			}
			void setMultiple()
			{
				minb = -1;
			}
			Path_message merge(const Path_message &u, const Path_message &v)
			{
				return Path_message(u.mina + v.mina, std::min(u.minb, v.minb));
			}
			Path_message better(const Path_message &u, const Path_message &v)
			{
				if(u.mina < v.mina) return u;
				else if(v.mina < u.mina) return v;
				else return Path_message(u.mina, -1);
			}
		};
		
		class Lcc_message
		{
		public:
			Path_message path_msg;
			Edge *first_edge, *last_edge;
			bool cir_flag, multi_flag;
			
			void make_rev()
			{
				std::swap(first_edge, last_edge);
			}
			void make_cir(Circle *cir, bool flag)
			{
                cir_flag = (!flag && cir != NULL);
                multi_flag = false;
                if(cir && first_edge -> getCir() != cir && last_edge -> getCir() != cir)
				{
				    //if(cir && !cir -> equal) printf("===================\n");
				    //if(cir && cir -> equal) printf("fine\n");
                    if(cir->equal)
                        multi_flag = true;
                }
			}
			void make_add(int delta, bool flag)
			{
				if(!flag)
                    path_msg.minb += delta;
			}
			friend inline Lcc_message operator+(const Lcc_message &u, const Lcc_message &v)
			{
                Lcc_message res;
                Edge *e = u.last_edge;
                res.path_msg = res.path_msg.merge(res.path_msg.merge(u.path_msg, Path_message(e->w)), v.path_msg);
                res.multi_flag = (u.multi_flag || v.multi_flag);
                if(e -> cir && u.first_edge -> getCir() != e -> cir && v.last_edge-> getCir() != e->cir)
				{
                    if(e -> cir -> equal)
                        res.multi_flag = true;
                }
                res.first_edge = u.first_edge;
                res.last_edge = v.last_edge;
                res.cir_flag = u.cir_flag || e -> cir || v.cir_flag;
                return res;
			}
		};
		
		class Node
		{
		public:
			int id;
			Node *par, *ls, *rs;
			Edge *pre, *nex; 
			Circle *cir;
		
			Lcc_message message;
			bool rev_flag, cir_flag;
		
			int delta;
		
			bool is_root()
			{
				return !(par && (par->ls == this || par->rs == this));
			}
			void make_rev()
			{
				rev_flag ^= 1;
				message.make_rev();
			}
			void make_cir(Circle *tmpcir)
			{
				cir_flag = true;
				cir = tmpcir;
				message.make_cir(tmpcir, !ls && !rs);
			}
			void make_add(int tmp)
			{
				delta += tmp;
				message.make_add(tmp, !ls && !rs);
			}
			void pushdown()
			{
                if(rev_flag)
				{
                    std::swap(ls, rs);
                    std::swap(pre, nex);
                    if(ls)
					{
                        ls -> make_rev();
                    }
                    if(rs)
					{
                        rs -> make_rev();
                    }
                    rev_flag = false;
                }
                if(cir_flag)
				{
                    if(ls)
					{
                        pre -> cir = cir;
                        ls -> make_cir(cir);
                    }
                    if(rs)
					{
                        nex -> cir = cir;
                        rs -> make_cir(cir);
                    }
                    cir_flag = false;
                }
                if(delta != 0)
				{
                    if(ls)
					{
                        pre -> w.wb += delta;
                        ls -> make_add(delta);
                    }
                    if(rs)
					{
                        nex -> w.wb += delta;
                        rs -> make_add(delta);
                    }
                    delta = 0;
                }
			}
			void maintain()
			{
                message.path_msg.setEmpty();
                message.first_edge = pre;
                message.last_edge = nex;
                message.cir_flag = false;
                message.multi_flag = false;
                if(ls)
                    message = ls -> message + message;
                if(rs)
                    message = message + rs -> message;
			}
			void all_pushdown()
			{
				std::vector<Node*> tmp;
				Node *t = NULL;
				for (Node *i = this; !i -> is_root(); i = i -> par)
					tmp.push_back(i), t = i;
				if(t && t -> is_root()) t -> pushdown();
				else if(t && t -> par && t -> par -> is_root()) t -> par -> pushdown();
				//tmp.push_back(tmp[tmp.size() - 1] -> par);
				for (int i = tmp.size() - 1; i >= 0; i--)
					tmp[i] -> pushdown();
				tmp.clear();
			}
			void zig(/*Node *x*/)
			{
				Node *fa = par;
				if (par -> par)
				{
					if (par -> par -> ls == par) par -> par -> ls = this;
					else if (par -> par -> rs == par) par -> par -> rs = this;
				}
				par = fa -> par;
				fa -> par = this;
				fa -> ls = rs;
				if (rs) rs -> par = fa;
				rs = fa;

				fa -> maintain();//
				maintain();//
			}
		
			void zag(/*Node *x*/)
			{
				Node *fa = par;
				if (par -> par)
				{
					if (par -> par -> ls == par) par -> par -> ls = this;
					else if (par -> par -> rs == par) par -> par -> rs = this;
				}
				par = fa -> par;
				fa -> par = this;
				fa -> rs = ls;
				if (ls) ls -> par = fa;
				ls = fa;

				fa -> maintain();//
				maintain();//
			}
		
			void rotate()
			{
				if(this == par -> ls) zig();
				else zag();
			}
		
			void splay()
			{
				all_pushdown();
				while(!is_root())
				{
					if(par -> is_root()) rotate();
					else
					{
						bool t1 = (par -> ls == this), t2 = (par -> par -> ls == par);
						if(t1 == t2) par -> rotate();
						else rotate();
						rotate();
					}
				
				}
			}
		    void splay_until(Node *x) 
			{
				all_pushdown();
				while(par != x)
				{
					if(par -> par != x)
					{
						bool t1 = (par -> ls == this), t2 = (par -> par -> ls == par);
						if(t1 == t2) par -> rotate();
						else rotate();	
					}
					rotate();
				}
			}
			int get_disL() 
			{
				if(!pre) 
				{
					return 0;
				}
				int ans = pre -> w.wa;
				if(ls) 
				{
					ans += ls -> message.path_msg.mina + message.first_edge -> w.wa;
				}
				return ans;
			}
			
			int get_disR() 
			{
				if(!nex) 
				{
					return 0;
				}
				int ans = nex -> w.wa;
				if(rs) 
				{
					ans += rs -> message.path_msg.mina + message.last_edge -> w.wa;
				}
				return ans;
			}
			void access()
			{
				Node *x = this;
				Node *now = x, *father = NULL;
				bool checkflag;
				Circle *tmpcir = NULL;
			 	for(; now; father = now, now = now -> par) 
				{
			    	now -> splay();
			        if(now -> pre && now -> pre -> cir) 
					{
			            checkflag = false;
			            tmpcir = now -> pre -> cir;
			            if(tmpcir -> pex && !tmpcir -> pex -> is_root())
			                tmpcir -> pex = now;
						tmpcir -> pb -> splay();
						tmpcir -> pa -> splay();
						if(tmpcir -> pb -> is_root()) 
						{
							if(tmpcir -> pb -> par != tmpcir -> pa) 
							{
								std::swap(tmpcir -> pa, tmpcir -> pb);
								if(tmpcir -> pex) 
								{
									tmpcir -> pex -> make_rev(); 
								}
							}
						}
						else 
						{
							checkflag = true;
							tmpcir -> pb -> splay_until(tmpcir -> pa);
							if(tmpcir -> pa -> ls == tmpcir -> pb) 
							{
								tmpcir -> pb -> rotate();
								std::swap(tmpcir -> pa, tmpcir -> pb);
								if(tmpcir -> pex) 
								{
									tmpcir -> pex -> make_rev();
								}
							}
							tmpcir -> pa -> rs = NULL;
							tmpcir -> pa -> nex = NULL;
						}
						tmpcir -> pb -> rs = tmpcir -> pex;
						if(tmpcir -> pex) 
						{
							tmpcir -> pb -> nex = tmpcir -> pex -> message.first_edge;
						} 
						else 
						{
							tmpcir -> pb -> nex = tmpcir -> miss;
						}	
						if(tmpcir -> pex) 
						{
							tmpcir -> pex -> par = tmpcir -> pb;
						}
						now -> splay();
						if(now -> get_disL() > now -> get_disR()) 
						{
							now -> make_rev();
							now -> pushdown();
						}
						tmpcir -> pb = now;
						tmpcir -> pex = now -> rs;
						if(now -> rs) 
						{
							tmpcir -> miss = NULL;
						} 
						else 
						{
							tmpcir -> miss = now -> nex;
						}
						tmpcir -> equal = now -> get_disL() == now -> get_disR();
						if(tmpcir -> pex) 
						{
							tmpcir -> pex -> par = NULL;
						}
						now -> rs = father;
						if(father) 
						{
							now -> nex = father -> message.first_edge;
						}	 
						else 
						{
							now -> nex = NULL;
						}
						now -> maintain();
						if(checkflag) 
						{
							tmpcir -> pa -> rs = now;
							tmpcir -> pa -> nex = now -> message.first_edge;
							now -> splay();
						}
					} 
					else 
					{
						now -> rs = father;
						if(father) 
						{
							now -> nex = father -> message.first_edge;
						} 
						else 
						{
							now -> nex = NULL;
						}
						now -> maintain();
					}
				}
				splay();
			}
			Node *get_root()
			{
				access();
				Node *x = this;
				while(x -> ls)
				{
					x->pushdown(), x = x -> ls;
				}
				x -> splay();
				return x;
			}
			void evert()
			{
				access(), make_rev(), pushdown();
			}
		};
		class Weight
		{
		public:
			int wa, wb;
			friend bool operator == (const Weight &u, const Weight &v)
			{
				return (u.wa == v.wa && u.wb == v.wb);
			}
			friend bool operator != (const Weight &u, const Weight &v)
			{
				return (u.wa != v.wa || u.wb != v.wb);
			}
		
		};
		class Edge
		{
		public:
			int id;
			Weight w;
			Circle *cir;
			Circle *getCir()
			{
				return this ? this -> cir : NULL;
			}//
		};
		class Circle
		{
		public:
			int id;
			Node *pa, *pb, *pex;
			Edge *miss;
			bool equal;
		};
		
		
		Node **node;
		Edge **edge;
		Circle **circle;
		size_t node_size, edge_size, circle_size;
		Link_cut_cactus() {}
		Link_cut_cactus(const int &n)
		{
			node_size = n;
			edge_size = 0;
			circle_size = 0;
			node = new Node* [n + 10];
			for (int i = 0; i <= n; i++)
				node[i] = new Node();
			edge = new Edge* [2 * n + 10];
			circle = new Circle* [2 * n + 10];
		}
		
		~Link_cut_cactus()
		{
			for (int i = 0; i <= node_size; i++)
				if(node[i]) delete(node[i]);
			delete  node;
			for (int i = 0; i <= edge_size; i++)
				if(edge[i]) delete(edge[i]);
			delete  edge;
			for (int i = 0; i <= circle_size; i++)
				if(circle[i]) delete(circle[i]);
			delete  circle;
		}
		bool link(int u, int v, int wa, int wb)
		{
			if (u == v) return false;

			Weight w;
			w.wa = wa, w.wb = wb;
			
			Node *x = node[u], *y = node[v];
			x -> evert(), y -> evert();
			if (x -> par)
			{
				//x and y are in the same cactus
				x -> access();
				//if they're already in a circle: invalid
				if (x -> message.cir_flag)
					return false;
				//generate a circle
				Circle *cir = new Circle;
				circle[++circle_size] = cir;
				Edge *e = new Edge;
				edge[++edge_size] = e;
				e -> w = w, e -> cir = cir;
				cir -> pa = y, cir -> pb = x, cir -> pex = NULL;
				cir -> miss = e;
				x -> make_cir(cir);
                x -> pushdown();
				x->access();
			}
			else
			{
				//not connected: the same case as LCT.
				Edge *e = new Edge;
				edge[++edge_size] = e;
				e -> w = w; e -> cir = NULL;
				x -> par = y; x -> pre = e; x -> maintain();
			}
			return true;
		}
		bool cut(int u, int v, int wa, int wb)
		{
			if (u == v) return false;

			Weight w;
			w.wa = wa, w.wb = wb;

			Node *x = node[u], *y = node[v];
			if (x -> get_root() != y -> get_root())
				return false;

			y -> evert(), x -> access();
			y -> splay_until(x);

			Circle *cir = x -> pre -> cir;
			if (cir && cir -> pa == y && !cir -> pex && cir -> miss -> w == w)
			{
				//x,y in the same circle, 'missing edge' case, and the missing edge is exactly the edge to delete:
				x -> make_cir(NULL);
				return true;
			}

			if (!y -> rs && x -> pre -> w == w){
				if (cir)
				{
					if (cir -> pex)
					{
						//the extra path should be added into current path.
						cir -> pex -> make_rev();

						cir -> pex -> par = y, y -> rs = cir -> pex;
						y -> nex = cir -> pex -> message.first_edge;
						x -> pre = cir -> pex -> message.last_edge;
					}
					else{
						//simply delete it
						y -> nex = x -> pre = cir -> miss;
					}

					y->maintain(), x->maintain();
					x->make_cir(NULL); x -> pushdown();
				}
				else{
					//no circle: LCT case.
					y->par = NULL, y->nex = NULL, y->maintain();
					x->ls = NULL, x->pre = NULL, x->maintain();
				}
				return true;
			}
			return false;
		}
		bool add(int u, int v, int delta)
		{
			Node *x = node[u], *y = node[v];
			if (x->get_root() != y->get_root())
				return false;

			x->evert(), y->access();
			if (y->message.multi_flag)
				return false;
			y->make_add(delta); y -> pushdown();
			return true;
		}
		std::pair<int, int> query(int u, int v)
		{
			std::pair<int, int> ret;
			//case v = u
			if (u == v){
				ret.first = 0, ret.second = 2147483647;
				return ret;
			}
			//case unconnected
			Node *x = node[u], *y = node[v];
			if (x->get_root() != y->get_root()){
				ret.first = -1, ret.second = -1;
				return ret;
			}
			
			x->evert(), y->access();
			Path_message tmp = y->message.path_msg;
			//special judge: case multiway.
			if (y->message.multi_flag){
				tmp.minb = -1;
			}
			ret.first = tmp.mina;
			ret.second = tmp.minb;
			return ret;
		}
	};

Link_cut_cactus lcc(200005);
int main()
{
	freopen("in", "r", stdin);
	int n, Q;
	scanf("%d%d", &n, &Q);
	while(Q--)
	{
		char op[20];
		int u, v, wa, wb, delta;
		scanf("%s", op);
		if(op[0] == 'l')
		{
			scanf("%d%d%d%d", &v, &u, &wa, &wb);
			if(lcc.link(u, v, wa, wb)) printf("ok\n");
			else printf("failed\n");
		}
		else if (op[0] == 'c')
		{
			scanf("%d%d%d%d", &u, &v, &wa, &wb);
			if (lcc.cut(v, u, wa, wb)) printf("ok\n");
			else printf("failed\n");
		}
		else if (op[0] == 'a')
		{
			scanf("%d%d%d", &u, &v, &delta);
			if (lcc.add(v, u, delta)) printf("ok\n");
			else printf("failed\n");
		}
		else if (op[0] == 'd')
		{
			scanf("%d%d", &u, &v);
			
			std::pair<int, int>ret = lcc.query(v, u);
			printf("%d %d\n", ret.first, ret.second);
		}
		else
		{
			printf("error!\n");
		}
	}
	return 0;
	
}
