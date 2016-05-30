#ifndef DATA_STRUCTURE_LINK_CUT_CACTUS
#define DATA_STRUCTURE_LINK_CUT_CACTUS

#include <functional>
#include <cstddef>
#include <cassert>
namespace DataStructure
{
	template<class T>
	class Link_cut_cactus
	{
	public:
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
		
			bool is_root(Node *x)
			{
				return !(x -> par && (x -> par -> ls == x || x -> par -> rs == x));
			}
			void make_rev()
			{
				rev_flag ^= 1;
				message.make_rev();
			}
			void make_cir(Circle *cir)
			{
				cir_flag = true;
				cir = cir;
				message.make_cir(cir, !ls && !rs);
			}
			void make_add(int delta)
			{
				delta += delta;
				message.make_add(delta, !ls && !rs);
			}
            void pushdown()
			{
                if(rev_flag){
                    std::swap(ls, rs);
                    std::swap(pre, nex);
                    if(ls){
                        ls->make_rev();
                    }
                    if(rs){
                        rs->make_rev();
                    }
                    rev_flag = false;
                }
                if(cir_flag){
                    if(ls){
                        pre->cir = cir;
                        lc->make_cir(cir);
                    }
                    if(rs){
                        pre->cir = cir;
                        rs->make_cir(cir);
                    }
                    cir_flag = false;
                }
                if(delta != 0){
                    if(ls){
                        pre->w.wb += delta;
                        ls->make_add(delta);
                    }
                    if(rs){
                        pre->w.wb += delta;
                        rs->make_add(delta);
                    }
                    delta = 0;
                }
			}
			void maintain()
			{
                message.Path_message.setEmpty();
                message.first_edge = pre;
                message.last_edge = nex;
                message.cir_flag = false;
                message.multi_flag = false;
                if(ls)
                    message = ls->message + message;
                if(rs)
                    message = message + rs->message;
			}
			void all_pushdown()
			{
				vector<Node*> tmp;
				for (Node *i = this; !i -> is_root(); i = i -> fa)
					tmp.push_back(i);
				for (int i = tmp.size() - 1; i >= 0; i--)
					tmp[i] -> pushdown();
				tmp.clear();
			}
			void zig(Node *x)
			{
				Node *par = x -> par;
				if(par -> par)
				{
					if(par -> par -> ls == par) par -> par -> ls = x;
					else if(par -> par -> rs == par) par -> par -> rs = x;
				}
			
				x -> par = par -> par;
				par -> par = x;
				par -> ls = x -> rs;
				if(x -> rs) x -> rs -> par = par;
				x -> rs = par;
			
				maintain(par);//
				maintain(x);//
			}
		
			void zag(Node *x)
			{
				Node *par = x -> par;
				if(par -> par)
				{
					if(par -> par -> ls == par) par -> par -> ls = x;
					else if(par -> par -> rs == par) par -> par -> rs = x;
				}
			
				x -> par = par -> par;
				par -> par = x;
				par -> rs = x -> ls;
				if(x -> ls) x -> ls -> par = par;
				x -> ls = par;
			
				maintain(par);//
				maintain(x);//
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
					if(par -> is_root()) rotate(x);
					else
					{
						bool t1 = (par -> ls == this), t2 = (par -> par -> ls == par);
						if(t1 == t2) par -> rotate();
						else rotate();
						rotate();
					}
				
				}
			}
	
			void access()
			{
				//TODO?
			}
			Node *get_root()
			{
				access();
				Node *x = this;
				while(x -> ls)
				{
					pushdown(x), x = x -> ls;
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
			int wa; T wb;
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
			//
		};
		class Circle
		{
		public:
			int id;
			Node *pa, *pb, *pex;
			Edge *miss;
			bool equal;
		};
		class Path_message
		{
		public:
			int mina;
			T minb;
			
			Path_message() {}
			Path_message(const Weight &w)
			{
				mina = w.wa, minb = w.wb;
			}
			Path_message(const int &a, const T &b)
			{
				mina = a, minb = b;
			}
            void setEmpty(){
                mina = 0;
                minb = INF;
            }
            friend inline path_message operator*(const Path_messgae &u, const Path_message &v)
			{
				return Path_message(u.minA + v.minA, min(u.minB, v.minB));
			}
			friend inline path_message operator+(const Path_message &u, const Path_message &v)
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
                cir_flag = !flag && cir != NULL;
                multi_flag = false;
                if(cir && first_edge->cir != cir && last_edge->cir != cir){
                    if(cir->equal)
                        multi_flag = true;
                }
			}
			void make_add(T delta, bool flag)
			{
				if(!flag)
                    path_msg.minb += delta;
			}
			friend inline lcc_message operator+(const Lcc_message &u, const Lcc_message &v)
			{
                Lcc_message res;
                assert(u.last_edge == v.first_edge);
                Edge *e = u.last_edge;
                res.path_msg = u.path_msg * Path_messgae(e->w) * v.path_msg;
                res.multi_flag = u.multi_flag || v.multi_flag;
                if(e->cir && u.first_edge->cir != e->cir && v.last_edge->cir != e->cir){
                    if(e->cir->equal)
                        res.multi_flag = true;
                }
                res.first_edge = u.first_edge;
                res.last_edge = v.last_edge;
                res.cir_flag = u.cir_flag || e->cir || v.cir_flag;
                return res;
			}
		};
		Node **node, **edge, **circle;
		size_t node_size, edge_size, circle_size;
		Link_cut_cactus() {} = default;
		Link_cut_cactus(const int &n)
		{
			node_size = n;
			edge_size = 0;
			circle_size = 0;
			**node = new Node* [n + 10];
			for (int i = 0; i <= n; i++)
				node[i] = new Node();
			**edge = new Edge* [2 * n + 10];
			**circle = new Circle* [2 * n + 10];
		}
		
		~Link_cut_cactus
		{
			for (int i = 0; i <= node_sz; i++)
				if(node[i]) delete(node[i]);
			delete  node;
			for (int i = 0; i <= edge_sz; i++)
				if(edge[i]) delete(edge[i]);
			delete  edge;
			for (int i = 0; i <= circle_sz; i++)
				if(circle[i]) delete(circle[i]);
			delete  circle;
		}
		bool link(int u, int v, int wa, int wb)
		{
			//TODO
		}
		bool cut(int u, int v, int wa, int wb)
		{
			//TODO
		}
		bool add(int u, int v, int delta)
		{
			//TODO
		}
		pair<int, T> query(int u, int v)
		{
			//TODO
		}
	};
}
#endif
