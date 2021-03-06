#include "lib.h"

template<typename F>struct NoOp{void setup(ui, F){}void op(F&p,F n,ui,ui){p=n;}void down(F&,F&,F&,ui,ui) {}};

template<typename F,typename SetOp,typename PowerOp>struct Lazy{
    void setup(ui s,F def){this->def=def;this->s=s;L=new F[s]();fill(L,L+s,def);}
    void down(F&u,F&l,F&r,ui i,ui s){op(l,L[i],i<<1,s>>1);op(r,L[i],i<<1|1,s>>1);L[i]=def;}
    void op(F&p,F n,ui i,ui s){p=sop(p,pop(n,s));if(i<this->s)this->L[i]=sop(this->L[i],n);}
    SetOp sop;PowerOp pop;F*L;ui s;F def;
};

template <typename F, typename CombineOp, typename ModifyOp = NoOp<F>> struct SegTree {
	void setup(ui s, F def) {
		n = 1<<logceil(s);
		T = vector<F>(2*n, def);
		for (ui i = n-1; i > 0; i--) T[i] = cop(T[i<<1],T[i<<1|1]);
		mop.setup(2*n,def);
	}

	void setup(vector<F> & data, F def = F(), F def2 = F()) {
		n = 1<<logceil(data.size());
		T = vector<F>(2*n, def);
		copy(data.begin(), data.end(), T.begin() + n);
		for (ui i = n-1; i > 0; i--) T[i] = cop(T[i<<1],T[i<<1|1]);
		mop.setup(2*n,def2);
	}

	inline void put(ui x, F n) { put(x, x, n); }
	inline void put(ui from, ui to, F v) { put2(from, to+1, v, 1, n); }
	inline F get(ui x) { return get(x, x); }
	inline F get(ui from, ui to) { return get2(from, to+1, 1, n); }

	void put2(ui from, ui to, F v, ui i, ui s) {
		if (from == 0 && to == s) { mop.op(T[i], v, i, s); return; }
		mop.down(T[i], T[i<<1], T[i<<1|1], i, s);
        s>>=1;i<<=1;
        if (to <= s) { put2(from, to, v, i, s); }
        else if (from >= s) { put2(from-s, to-s, v, i|1, s); }
        else {
            put2(from, s, v, i, s);
            put2(0, to-s, v, i|1, s);
        }
		T[i>>1] = cop(T[i], T[i|1]);
	}

	F get2(ui from, ui to, ui i, ui s) {
        while (true) {
            if (from == 0 && to == s) return T[i];
            mop.down(T[i], T[i << 1], T[i << 1 | 1], i, s);
            s >>= 1;i <<= 1;
            if (to > s) {
                to -= s;
                if (from >= s) { from -= s; i|=1; }
                else return cop(get2(from, s, i, s), get2(0, to, i|1, s));
            }
        }
    }

	ui n;
	vector<F> T;
	CombineOp cop;
    ModifyOp mop;
};


template <typename F> struct AddOp { F operator()(F a, F b) { return a+b; }};
template <typename F> struct MinOp { F operator()(F a, F b) { return std::min(a,b); }};
template <typename F> struct MaxOp { F operator()(F a, F b) { return std::max(a,b); }};
template <typename F> struct MultiplyOp { F operator()(F a, F b) { return a*b; }};
template <typename F> struct MultOp { F operator()(F a, ui b) { return a*b; }};
template <typename F> struct IdempOp { F operator()(F a, ui b) { return a; }};
template <typename F> struct InverseOp { F operator()(F a, F b) { return b?b-a:a; }};

template<typename T> using AddSumTree = SegTree<T, AddOp<T>, Lazy<T, AddOp<T>, MultOp<T>>>;
template<typename T> using AddMaxTree = SegTree<T, MaxOp<T>, Lazy<T, AddOp<T>, IdempOp<T>>>;
template<typename T> using AddMinTree = SegTree<T, MinOp<T>, Lazy<T, AddOp<T>, IdempOp<T>>>;
// NOTE: set defaultValue properly in these (-INF for MAX and INF for MIN), otherwise you'll get weird results
template<typename T> using AssignMinTree = SegTree<T, MinOp<T>, Lazy<T, MinOp<T>, IdempOp<T>>>;
template<typename T> using AssignMaxTree = SegTree<T, MaxOp<T>, Lazy<T, MaxOp<T>, IdempOp<T>>>;
template<typename T> using XorTree = SegTree<T, AddOp<T>, Lazy<T, InverseOp<T>, MultOp<T>>>;

template<typename T> using SetMinTree = SegTree<T, MinOp<T>>;
template<typename T> using SetMaxTree = SegTree<T, MaxOp<T>>;
template<typename T> using SetMulTree = SegTree<T, MultiplyOp<T>>;
