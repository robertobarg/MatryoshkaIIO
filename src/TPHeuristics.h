///
/// src/algs/TPHeuristics.h
///
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// Copyright 2024 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
/// All rights reserved
///


#ifndef TPHEUR_H
#define TPHEUR_H

#include <chrono>

#include "TpInstance.h"
#include "TSimplex.h"
#include "optresult.h"


template<typename T, typename U>
class TProbHeuristics
{
public:
    TProbHeuristics(const T&, const std::vector<TpQuantityType>&, const std::vector<TpQuantityType>&);
    ~TProbHeuristics();
    
    virtual optresult run() = 0;
    
    std::shared_ptr<U> getSolution();
    
    std::shared_ptr<std::vector<NodeArcIdType>> getSolutionArcs();
    
protected:
    const T& tp_costs_ref;
    const std::vector<TpQuantityType>& q_at_srcs_ref;
    const std::vector<TpQuantityType>& q_at_dsts_ref;
    
    std::shared_ptr<U> quantities;
    std::shared_ptr<std::vector<NodeArcIdType>> solution;
};

template<typename T, typename U>
class NorthWestCorner : public TProbHeuristics<T, U>
{
public:
    NorthWestCorner(const T&, const std::vector<TpQuantityType>&, const std::vector<TpQuantityType>&);
    ~NorthWestCorner();
    
    optresult run();
    
    
private:
    using TProbHeuristics<T, U>::tp_costs_ref;
    using TProbHeuristics<T, U>::q_at_srcs_ref;
    using TProbHeuristics<T, U>::q_at_dsts_ref;
    
    using TProbHeuristics<T, U>::quantities;
    using TProbHeuristics<T, U>::solution;
};

///
/// classes implementation
///


template<typename T, typename U>
TProbHeuristics<T, U>::TProbHeuristics(const T& tcs, const std::vector<TpQuantityType>& srcs, const std::vector<TpQuantityType>& dsts)
    : tp_costs_ref(tcs)
    , q_at_srcs_ref(srcs)
    , q_at_dsts_ref(dsts)
{
    quantities.reset(new U(srcs.size() * dsts.size(), std::numeric_limits<TpQuantityType>::quiet_NaN()));
    solution.reset(new std::vector<NodeArcIdType>());
}

template<typename T, typename U>
TProbHeuristics<T, U>::~TProbHeuristics()
{
}

template<typename T, typename U>
std::shared_ptr<U> TProbHeuristics<T, U>::getSolution()
{
    return quantities;
}

template<typename T, typename U>
std::shared_ptr<std::vector<NodeArcIdType>> TProbHeuristics<T, U>::getSolutionArcs()
{
    return solution;
}

template<typename T, typename U>
NorthWestCorner<T, U>::NorthWestCorner(const T& tcs, const std::vector<TpQuantityType>& srcs, const std::vector<TpQuantityType>& dsts)
    : TProbHeuristics<T, U>(tcs, srcs, dsts)
{
}

template<typename T, typename U>
NorthWestCorner<T, U>::~NorthWestCorner()
{
}

#define GETOPTT(start) ((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0)
#define GETOPTTMS(start) double((std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()) / 1000.0)

template<typename T, typename U>
optresult NorthWestCorner<T, U>::run()
{
    /// local vars
    std::vector<TpQuantityType> q_at_srcs;
    std::vector<TpQuantityType> q_at_dsts;
    std::vector<bool> nodes;
    NodeArcIdType M;
    NodeArcIdType N;
    NodeArcIdType i;
    NodeArcIdType j;
    unsigned long iter;
    NodeArcIdType srcs_rem;
    NodeArcIdType dsts_rem;
    double min_q;
    double tot_cost;
    double tot_q;
    double opt_sec;

    /// start chrono
    auto start = std::chrono::steady_clock::now();
    
    /// init. data
    q_at_srcs = q_at_srcs_ref;
    q_at_dsts = q_at_dsts_ref;
    M = q_at_srcs.size();
    N = q_at_dsts.size();
    nodes = std::vector<bool>(M + N, true);
    
    /// clear solution before (re)compute
    solution->clear();
    
    /// main loop
    srcs_rem = dsts_rem = iter = i = j = 0;
    tot_cost = tot_q = 0.0;
    do
    {
        /// get min. q.
        min_q = std::min(q_at_srcs[i], q_at_dsts[j]);

        q_at_srcs[i] -= min_q;
        q_at_dsts[j] -= min_q;
        
        tot_q += min_q;
        tot_cost += tp_costs_ref[N * i + j] * min_q;
        quantities->set(N * i + j, min_q > MYEPS
                                   ? min_q
                                   :
                                   #ifdef EPSQIS0
                                   0.0
                                   #else
                                   EPSQ
                                   #endif
                                   );
        solution->push_back(N * i + j);
        
        if(std::round(q_at_srcs[i]) < MYEPS && std::round(q_at_dsts[j]) < MYEPS)
            nodes[dsts_rem > srcs_rem ? i : M + j] = false;
        else
            nodes[std::round(q_at_srcs[i]) < MYEPS ? i : M + j] = false;
        
        nodes[i] ? ++dsts_rem : ++srcs_rem;
        nodes[i] ? ++j : ++i;
        
        /// update iter
        ++iter;
    }
    while(i < M && j < N && iter < M + N - 1);
    
    /// get opt time
    opt_sec = GETOPTT(start);

    /// return value
    optresult optres;
    optres.obj_value = tot_cost;
    optres.run_time = opt_sec;
    optres.tags.resize(2);
    optres.tags[1] = std::string("NWC");
    optres.integer_values.resize(1);
    optres.integer_values[0] = iter;
    optres.double_values.resize(3, std::numeric_limits<double>::quiet_NaN());
    optres.double_values[0] = tot_q;
    optres.double_values[2] = GETOPTTMS(start);
        
    return optres;
}


#endif