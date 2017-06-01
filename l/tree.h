

#include <cstddef>
#include <vector>


template<typename T> struct Fenwick {
	explicit Fenwick(size_t N, T t=T()):F(N,t),N(N),t(t){}
	explicit Fenwick(const vector<T>&A, T t=T()):F(A),N(A.size()),t(t){
		for(int i=0;i<N;i++){int j=i+lsb(i+1);if(j<N)F[j]+=F[i];}}
	void add(int i, T v){while(i<=N){F[i]+=v;i+=lsb(i+1);}}
	T sum(int i)const{T sum(t);while(i){sum+=F[i-1];i-=lsb(i);}return sum;}
	T range(int i,int j)const{T s(t);while(j>i){s+=F[j-1];j-=lsb(j);}while(i>j){s-=F[i-1];i-=lsb(i);}return s;}
	T get(int i)const{return range(i,i+1);}
	constexpr int lsb(int i)const{return i&-i;}
	vector<T> F;size_t N;T t;
};

template<typename F>struct NoCombine {
    const int NOCOMB=-2;
    void setup(size_t){}void op(F&p,F n,size_t,size_t,size_t){p=n;}F up(F l,F r,size_t){return l==r?l:NOCOMB;}
    void down(F&u,F&l,F&r,size_t,size_t,size_t){if(u!=NOCOMB){l=r=u;}u=NOCOMB;}};
template<typename F,typename Op>struct Persistent{Op o;void setup(size_t){}void op(F&p,F n,size_t,size_t, size_t){p=n;}F up(F l, F r, size_t){return o(l,r);}void down(F&,F&,F&,size_t,size_t, size_t b) {}};

template<typename F,typename SetOp,typename CombineOp,typename PowerOp>struct LazyProp{
    void setup(size_t s){this->s=s;L=new F[s]();}
    void down(F&u,F&l,F&r,size_t i,size_t a,size_t b){op(l,L[i],2*i,a,(a+b)/2);op(r,L[i],2*i+1,(a+b)/2,b);L[i]=0;}
    void op(F&p,F n,size_t i,size_t a,size_t b){p=sop(p,pop(n,b-a));if(i<this->s)this->L[i]=sop(this->L[i],n);}
    F up(F l,F r,size_t){return cop(l,r);}SetOp sop;CombineOp cop;PowerOp pop;F*L;size_t s;
};

template <typename F> struct AddOp { F operator()(F a, F b) { return a+b; }};
template <typename F> struct MultOp { F operator()(F a, F b) { return a*b; }};
template <typename F> struct IdempOp { F operator()(F a, F b) { return a; }};
template <typename F> struct MinOp { F operator()(F a, F b) { return std::min(a,b); }};
template <typename F> struct MaxOp { F operator()(F a, F b) { return std::max(a,b); }};

template <typename F, typename Combine> struct SegTree {
	void setup(size_t s, F def) {
		size = 1<<logceil(s);
		T = vector<F>(2*size, def);
		for (size_t i = size-1; i > 0; i--) T[i] = op.up(T[2*i],T[2*i+1], i);
		op.setup(2*size);
	}

	void setup(vector<F> & data, F def = F()) {
		size = 1<<logceil(data.size());
		T = vector<F>(2*size, def);
		copy(data.begin(), data.end(), T.begin() + size);
		for (size_t i = size-1; i > 0; i--) T[i] = op.up(T[2*i],T[2*i+1], i);
		op.setup(2*size);
	}

	inline void put(size_t x, F n) { put(x, x, n); }
	inline void put(size_t from, size_t to, F n) { put2(from, to+1, n, 1, 0, size); }
	inline F get(size_t x) { return get(x, x); }
	inline F get(size_t from, size_t to) { return get2(from, to+1, 1, 0, size); }

	void put2(size_t from, size_t to, F n, size_t i, size_t a, size_t b) {
		if (from <= a && to >= b) { op.op(T[i], n, i, a, b); return; }
		if (from >= b || to <= a) { return; }
		op.down(T[i], T[2*i], T[2*i+1], i, a, b);
		put2(from, to, n, 2*i, a, (a+b)/2);
		put2(from, to, n, 2*i+1, (a+b)/2, b);
		T[i] = op.up(T[2*i], T[2*i+1], i);
	}

	F get2(size_t from, size_t to, size_t i, size_t a, size_t b) {
		if (from <= a && to >= b) { return T[i]; }
		op.down(T[i], T[2*i], T[2*i+1], i, a, b);
		if (to <= (a+b)/2) { return get2(from, to, 2*i, a, (a+b)/2); }
		else if (from >= (a+b)/2) { return get2(from, to, 2*i+1, (a+b)/2, b); }
		else return op.up(get2(from, to, 2*i, a, (a+b)/2), get2(from, to, 2*i+1, (a+b)/2, b), i);
	}

	size_t size;
	vector<F> T;
	Combine op;
};

template<typename T> using SetGetTree = SegTree<T, NoCombine<T>>;
template<typename T> using AddSumTree = SegTree<T, LazyProp<T, AddOp<T>, AddOp<T>, MultOp<T>>>;
template<typename T> using AddMaxTree = SegTree<T, LazyProp<T, AddOp<T>, MaxOp<T>, IdempOp<T>>>;
template<typename T> using AddMinTree = SegTree<T, LazyProp<T, AddOp<T>, MinOp<T>, IdempOp<T>>>;
template<typename T> using SetMinTree = SegTree<T, Persistent<T, MinOp<T>>>;
template<typename T> using SetMaxTree = SegTree<T, Persistent<T, MaxOp<T>>>;
