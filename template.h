#ifndef DATA_STRUCTURE_LINK_CUT_CACTUS
#define DATA_STRUCTURE_LINK_CUT_CACTUS

#include <functional>
#include <cstddef>
#include <cassert>
#include <vector>
namespace DataStructure
{
	template<class T>
	class Link_cut_cactus
	{
	public:
		class Node;
		class Edge;
		class Circle;
		class Lcc_message;

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
				//return !(x -> par && (x -> par -> ls == x || x -> par -> rs == x));
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
				//TODO
			}
			void maintain()
			{
				//TODO
			}
			void all_pushdown()
			{
				std::vector<Node*> tmp;
				for (Node *i = this; !i -> is_root(); i = i -> par)
					tmp.push_back(i);
				for (int i = tmp.size() - 1; i >= 0; i--)
					tmp[i] -> pushdown();
				tmp.clear();
			}
			void zig(/*Node *x*/)
			{
				if (par->par){
					if (par->par->ls == par) par->par->ls = this;
					else if (par->par->rs == par) par->par->rs = this;
				}
				par = par->par;
				par->par = this;
				par->ls = rs;
				if (rs) rs->par = par;
				rs = par;

				par->maintain();//
				maintain();//
				/*Node *par = x -> par;
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
			
				par->maintain();//
				x->maintain();//*/
			}
		
			void zag(/*Node *x*/)
			{
				if (par->par){
					if (par->par->ls == par) par->par->ls = this;
					else if (par->par->rs == par) par->par->rs = this;
				}

				par = par->par;
				par->par = this;
				par->rs = ls;
				if (ls) ls->par = par;
				ls = par;

				par->maintain();//
				maintain();//
				/*Node *par = x -> par;
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
			
				par->maintain();//
				x->maintain();//*/
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
				//TODO
			}
			void make_add(T delta, bool flag)
			{
				//TODO
			}
			void merge(const Lcc_message &u, const Lcc_message &v)
			{
				//TODO
			}
		};
		Node **node;
		Edge **edge;
		Circle **circle;
		size_t node_size, edge_size, circle_size;
		Link_cut_cactus() {} = default;
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
		std::pair<int, T > query(int u, int v)
		{
			//TODO
		}
	};
}
#endif
