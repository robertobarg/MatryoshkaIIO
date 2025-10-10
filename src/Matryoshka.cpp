///
/// src/algs/Matryoshka.cpp
///
/// This source file implements class Matryoshka, an algorithm for the resolution of image-processing instances of the optimal transport 
/// The algorithm solves successive transportation problems by means of the iterated inside-out algorithm by R. Bargetto, F. Della Croce, R. Scatamacchia
/// Iterated inside-out algorithm: https://arxiv.org/pdf/2302.10826.pdf
///
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// -------------------
///     The implementation of method 'Matryoshka::init_sol(..)' is derived from a routine written by Rosario Scatamacchia
///     DIGEP, Politecnico di Torino
///     Copyright 2024 by Rosario Scatamacchia
///     All rights reserved
/// -------------------
///
/// Copyright 2024 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
/// All rights reserved
///


#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

#include "Matryoshka.h"
#include "TPHeuristics.h"
#include "TSimplex.h"

#define GETOPTT(start) ((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0)
#define GETOPTTMS(start) double((std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()) / 1000.0)

//#define TESTDBG true

#define MINSZ 16

Matryoshka::Matryoshka(const unsigned int lvls, const std::shared_ptr<TpInstance::TProblemData>& tpd_ptr, bool vrb)
    : levels(lvls)
    , tpdata_l0_ptr(tpd_ptr)
    , verbose(vrb)
{
    if(!levels)
    {
        /// auto
        NodeArcIdType S = std::sqrt(tpdata_l0_ptr->n);
        while(S > MINSZ)
        {
            ++levels;
            S = S / 2;
        }
    }
}

Matryoshka::~Matryoshka()
{
}

optresult Matryoshka::run(const optcfg& opt_config)
{
    /// local vars
    std::vector<std::shared_ptr<TpInstance::TProblemData>> tprob_data_ptrs;
    std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>> sol_sptr;
    std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>> sol_arcs_sptr;
    std::shared_ptr<TSimplexData::ts_sol> qs_sptr;

    Shielding::THEgrid small_board;
    Shielding::THEgrid big_board;
    optresult optres;
    optresult initsol_optres;
    
    /// time tracing data
    std::vector<std::vector<double>> timing(levels + 2, std::vector<double>(3, std::numeric_limits<double>::quiet_NaN()));
    
    /// start timer
    auto start = std::chrono::steady_clock::now();
    
    /// init. stack of problems
    tprob_data_ptrs.push_back(tpdata_l0_ptr);
    
    /// generated reduced problems
    for(unsigned int l = 0; l < levels; l++)
        tprob_data_ptrs.push_back(reduceProbSize(tprob_data_ptrs[l]));
        
    /// tracing time
    timing[0][2] =GETOPTTMS(start);
    
    /// reverse prob. order
    std::reverse(tprob_data_ptrs.begin(), tprob_data_ptrs.end());
    
    /// main loop
    FILE_LOG(logINFO) << "Matryoshka ::: " << levels << " kuklas";
    for(unsigned int l = 0; l < tprob_data_ptrs.size(); l++)
    {
        /// logging
        FILE_LOG(logINFO) << "Matryoshka ::: " << "kukla " << l << " ::: "
                          << "solve problem of size " << tprob_data_ptrs[l]->m << "x" << tprob_data_ptrs[l]->n;
        
        /// init. alg
        std::shared_ptr<TpInstance::TProblemData> curr_tpdata_sptr;
        #ifdef REDINST
        curr_tpdata_sptr.reset(new TpInstance::TProblemData(*tprob_data_ptrs[l]));
        if(l < tprob_data_ptrs.size())
        {
            bool ired = curr_tpdata_sptr->reduce();
            if(ired)
            {
                FILE_LOG(logINFO) << "Matryoshka ::: " << "kukla " << l << " ::: fictious srcs and dsts removed ::: "
                                  << "problem size reduced to " << curr_tpdata_sptr->m << "x" << curr_tpdata_sptr->n;
            }
        }
        #else
        curr_tpdata_sptr = tprob_data_ptrs[l];
        #endif
        TSimplex tspx(curr_tpdata_sptr, opt_config.alg_mode, opt_config.intp0, opt_config.intp1, opt_config.dblp0);
        /// set sol il avail.
        auto setsol = std::chrono::steady_clock::now();
        if(l)
            tspx.setSolution(sol_arcs_sptr, qs_sptr, initsol_optres, false);
        timing[l + 1][0] = GETOPTTMS(setsol);
        
        /// solve
        optres = tspx.tsimplex(opt_config.timelimsec, l == levels ? true : false, l == levels ? true : false);
        if(l + 1 < tprob_data_ptrs.size())
            sol_sptr = tspx.getSolutionArcs(true, true);
        #ifdef TESTDBG
        if(l + 1 < tprob_data_ptrs.size())
        {
            std::shared_ptr<TSimplexData::ts_sol> test_qs_sptr = tspx.getBasis();
            qs_sptr.reset(new TSimplexData::ts_sol(tprob_data_ptrs[l]->m * tprob_data_ptrs[l]->n, std::numeric_limits<TpQuantityType>::quiet_NaN()));
            
            for(auto it = sol_sptr->begin(); it != sol_sptr->end(); it++)
            {
                NodeArcIdType i = curr_tpdata_sptr->sources_map_bw[it->first];
                NodeArcIdType j = curr_tpdata_sptr->destinations_map_bw[it->second];
                
                qs_sptr->set(it->first * tprob_data_ptrs[l]->n + it->second, 
                             (!i || !j) ? 0.0 : test_qs_sptr->get((i - 1) * curr_tpdata_sptr->n + (j - 1)));
            }
        }
        #endif
        
        if(l + 1 < tprob_data_ptrs.size())
        {
            FILE_LOG(logINFO) << "Matryoshka ::: Compute inital solution ::: search " 
                              << (tprob_data_ptrs[l + 1]->m + tprob_data_ptrs[l + 1]->n - 1) << " arcs ...";
            ///
            initsol_optres = init_sol(*sol_sptr, tprob_data_ptrs[l], tprob_data_ptrs[l + 1], sol_arcs_sptr, qs_sptr);
            timing[l + 1][0] += initsol_optres.double_values[2];
            initsol_optres.double_values[2] = GETOPTTMS(start);
        }
        
        timing[l + 1][1] = optres.double_values[8];
        timing[l + 1][2] = GETOPTTMS(start);
        
        /// pop problem
        tprob_data_ptrs[l].reset();
    }
    
    FILE_LOG(logINFO) << "Matryoshka ::: total time >> " << std::setprecision(1) << std::fixed << GETOPTT(start) << " [s]";
    FILE_LOG(logINFO) << "Matryoshka ::: details ...";
    FILE_LOG(logINFO) << std::setw(8) << "Kukla" 
                      << std::setw(24) << "Matry time [ms]"
                      << std::setw(24) << "TP time [ms]" 
                      << std::setw(24) << "Tot time [ms]";
    unsigned int l = 0;
    for(auto it = timing.begin(); it != timing.end(); it++)
    {
        FILE_LOG(logINFO) << std::setw(8) << l++ 
                          << std::setw(24) << std::fixed << std::setprecision(1) << it->at(0) 
                          << std::setw(24) << std::fixed << std::setprecision(1) << it->at(1) 
                          << std::setw(24) << std::fixed << std::setprecision(1) << it->at(2);
    }
    
    return optres;
}

void convert_size_x2(const NodeArcIdType pixel,
                     const NodeArcIdType size1,
                     const NodeArcIdType size2,
                     std::vector<NodeArcIdType>& tuple)
{
    /// clear 
    tuple.clear();
    /// fill
    NodeArcIdType first = 2 * (pixel / size1) * size2 + 2 * (pixel % size1);
    tuple.push_back(first);
    tuple.push_back(first + 1);
    tuple.push_back(first + size2);
    tuple.push_back(first + size2 + 1);
}

void convert_size(const Shielding::THEgrid& board,
                  const Shielding::THEgrid& twoboard,
                  const NodeArcIdType index,
                  const NodeArcIdType twogridsize,
                  std::vector<NodeArcIdType>& tuple)
{
    /// clear 
    tuple.clear();
    /// fill
    NodeArcIdType first = twoboard.table[2 * board.cell[index].row][2 * board.cell[index].col];
    tuple.push_back(first);
    tuple.push_back(first + 1);
    tuple.push_back(first + twogridsize);
    tuple.push_back(first + twogridsize + 1);
}

std::shared_ptr<TpInstance::TProblemData> Matryoshka::reduceProbSize(const std::shared_ptr<TpInstance::TProblemData>& tpd_sptr)
{
    /// local vars
    std::shared_ptr<TpInstance::TProblemData> red_pdat_sptr;
    NodeArcIdType S0;
    NodeArcIdType S1;
    NodeArcIdType M;
    NodeArcIdType N;
    const unsigned int K = 4;
    
    /// init. values
    red_pdat_sptr.reset(new TpInstance::TProblemData(*tpd_sptr));
    M = (red_pdat_sptr->m /= K);
    N = (red_pdat_sptr->n /= K);
    S0 = std::sqrt(tpd_sptr->n);
    S1 = std::sqrt(red_pdat_sptr->n);

    /// reinit. reduced problem sources and destination
    red_pdat_sptr->sources.clear();
    red_pdat_sptr->sources.resize(M, 0.0);
    red_pdat_sptr->destinations.clear();
    red_pdat_sptr->destinations.resize(N, 0.0);
    
    /// get tuple
    red_pdat_sptr->srcs_at_zero = 0;
    red_pdat_sptr->dsts_at_zero = 0;

    std::vector<NodeArcIdType> tuple;
    for (NodeArcIdType i = 0; i < M; i++)
    {
        convert_size_x2(i, S1, S0, tuple);
        
        for(unsigned int k = 0; k < K; k++)
        {
            if(tpd_sptr->sources[tuple[k]] > MYEPS)
                red_pdat_sptr->sources[i] += tpd_sptr->sources[tuple[k]];
        }
        if(red_pdat_sptr->sources[i] < MYEPS)
            ++red_pdat_sptr->srcs_at_zero;
        
        for(unsigned int k = 0; k < K; k++)
        {
            if(tpd_sptr->destinations[tuple[k]] > MYEPS)
                red_pdat_sptr->destinations[i] += tpd_sptr->destinations[tuple[k]];
        }
        if(red_pdat_sptr->destinations[i] < MYEPS)
            ++red_pdat_sptr->dsts_at_zero;
    }

    /// t. costs
    #ifdef EDOTF
    red_pdat_sptr->costs = TpInstance::euclidean_dist(red_pdat_sptr->n, tpd_sptr->cost_f);
    red_pdat_sptr->minc = 0;
    red_pdat_sptr->maxc = 2 * std::pow(std::sqrt(red_pdat_sptr->n) - 1, 2.0);
    red_pdat_sptr->avgc = 0.0;
    #else
    red_pdat_sptr->costs = TpInstance::tcosts(red_pdat_sptr->m * red_pdat_sptr->n, 0.0);
    red_pdat_sptr->minc = std::numeric_limits<TpCostType>::max();
    red_pdat_sptr->maxc = 0;
    red_pdat_sptr->avgc = 0.0;
    
    TpInstance::euclidean_dist ecd(red_pdat_sptr->n, tpd_sptr->cost_f);
    for(NodeArcIdType e = 0; e < ecd.size(); e++)
    {
        red_pdat_sptr->costs[e] = ecd[e];
        red_pdat_sptr->minc = std::min(red_pdat_sptr->minc, red_pdat_sptr->costs[e]);
        red_pdat_sptr->maxc = std::max(red_pdat_sptr->maxc, red_pdat_sptr->costs[e]);
        red_pdat_sptr->avgc += 1.0 / double(ecd.size()) * red_pdat_sptr->costs[e];
    }
    
    //throw std::runtime_error("Matryoshka works only if you compile with EDOTF flag defined");
    #endif
    
    #ifdef MONGE
    red_pdat_sptr->computeSequences();
    #endif
    
    return red_pdat_sptr;
}

optresult
Matryoshka::
init_sol(const std::vector<std::pair<NodeArcIdType, NodeArcIdType>>& solution,
         const std::shared_ptr<TpInstance::TProblemData>& tpd_sptr,
         const std::shared_ptr<TpInstance::TProblemData>& twotpd_sptr,
         std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>>& sol_arcs_sptr,
         std::shared_ptr<TSimplexData::ts_sol>& sol_sptr)
{
    /// start timer
    auto start = std::chrono::steady_clock::now();    
    sol_arcs_sptr.reset(new std::vector<std::pair<NodeArcIdType, NodeArcIdType>>());
    
    /// local vars
    NodeArcIdType M = tpd_sptr->m; 
    NodeArcIdType N = tpd_sptr->n; 
    NodeArcIdType S1 = std::sqrt(tpd_sptr->n); 
    NodeArcIdType M2 = twotpd_sptr->m; 
    NodeArcIdType N2 = twotpd_sptr->n;
    NodeArcIdType S2 = std::sqrt(twotpd_sptr->n); 
    NodeArcIdType K = 4; 
    NodeArcIdType srcs_rem = 0;
    NodeArcIdType dsts_rem = 0;

    std::vector<NodeArcIdType> tupleori;
    std::vector<NodeArcIdType> tupledest;

    std::vector<NodeArcIdType> degrows(M, 0);
    std::vector<NodeArcIdType> degcols(N, 0);
    
    std::vector<TpQuantityType> ORai(K, 0);
    std::vector<TpQuantityType> ORbj(K, 0);
    std::vector<Shielding::BV> BasicVars(solution.size());
    
    std::vector<TpQuantityType> q_at_srcs = twotpd_sptr->sources;
    std::vector<TpQuantityType> q_at_dsts = twotpd_sptr->destinations;
    TpQuantityType Q = 0;
    for(auto it = q_at_srcs.begin(); it != q_at_srcs.end(); it++)
        Q += *it;
    
    std::shared_ptr<TSimplexData::tsimplex_dense_sol> sol16;
    std::shared_ptr<std::vector<NodeArcIdType>> sol16arcs;
    
    std::vector<bool> nodes(twotpd_sptr->m + twotpd_sptr->n, true);
    std::vector<std::pair<NodeArcIdType, NodeArcIdType>> deg_arcs;
    TpQuantityType tot_q = 0;
    TpCostType cost = 0;
    NodeArcIdType added_vars = 0;
    unsigned long iter = 0;

    // BV contiene le informazioni su una variabile di base del problema più piccolo
    // Per una variabile con indici i e j, l'attributo degree è uguale al minimo tra il numero di variabili di base sulla riga i 
    // e il numero di variabili sulla colonna j. Se degree = 1, posso processare il relativo quadrato 4 X 4 per il problema più grande 
    //FILE_LOG(logINFO) << "Matryoshka ::: " << solution.size() << " input arcs";
    //FILE_LOG(logINFO) << "Matryoshka ::: " << std::setprecision(0) << std::fixed << Q << " quantity to transport";

    for (NodeArcIdType e = 0; e < solution.size(); e++)
    {
        BasicVars[e].row = solution[e].first;
        BasicVars[e].col = solution[e].second;
        
        BasicVars[e].degree = 0;
        
        // serve per l'aggiornamento progressivo dei degree.
        degrows[BasicVars[e].row]++;
        degcols[BasicVars[e].col]++;
    }

    for (NodeArcIdType e = 0; e < BasicVars.size(); e++)
        BasicVars[e].degree = std::min(degrows[BasicVars[e].row], degcols[BasicVars[e].col]);

    // Ordino le variabili per degree crescenti (è sufficiente ordinare solo una volta)
    std::sort(BasicVars.begin(), BasicVars.end(), [](const Shielding::BV& a, const Shielding::BV& b) { return a.degree < b.degree; });
    #ifdef TESTDBG
    TpQuantityType checkv = 0.0;
    FILE_LOG(logINFO) << "Matryoshka ::: CHECK input sol ...";
    checkv = 0.0;
    for(auto it = solution.begin(); it != solution.end(); it++)
        checkv += sol_sptr->get(it->first * N + it->second), ++added_vars;
    FILE_LOG(logINFO) << "Matryoshka ::: transhipped quantity check value >>> " << std::setprecision(0) << std::fixed << checkv;
    added_vars = 0;
    #endif
    /// 
    sol_sptr.reset(new TSimplexData::ts_sol(M2 * N2, std::numeric_limits<TpQuantityType>::quiet_NaN()));
    // Processo le variabili di base con degree = 1 una alla volta
    Shielding::BV thevar;
    NodeArcIdType h;
    bool stop;
    bool found;
    bool no_spat;
    no_spat = found = stop = false;
    do
    {
        /// search next variable
        if(!no_spat)
        {
            /// HERE time consuming ops
            found = false;
            for(h = 0; h < BasicVars.size(); h++)
            {
                if(BasicVars[h].degree == 1 || no_spat)
                {
                    thevar = BasicVars[h];
                    degrows[thevar.row]--;
                    degcols[thevar.col]--;
                    // Aggiornamento dei degree delle variabili di base rimaste
                    for (NodeArcIdType k = 0; k < BasicVars.size(); k++)
                        BasicVars[k].degree = std::min(degrows[BasicVars[k].row], degcols[BasicVars[k].col]);

                    BasicVars[h].degree = std::numeric_limits<NodeArcIdTypeSGND>::max();
                    found = true;
                    
                    break;
                }
            }
            /// END HERE
        }
        else
        {
            thevar = BasicVars[h++ % BasicVars.size()];
            if(h == BasicVars.size())
            {
                FILE_LOG(logINFO) << "Matryoshka ::: initial sol. round 2 ::: iter. " << iter << " ::: Missing " << (M2 + N2 - 1 - added_vars)
                                  << " arcs to transport quantity " << std::setprecision(0) << std::fixed << (Q - tot_q) << " ...";
                ///
                break;
            }
        }
        
        /// if variable not found and spat not complete
        if(!no_spat && !found && added_vars < M2 + N2 - 1)
        {
            FILE_LOG(logINFO) << "Matryoshka ::: initial sol. round 1 ::: iter. " << iter << " ::: Missing " << (M2 + N2 - 1 - added_vars)
                              << " arcs to transport quantity " << std::setprecision(0) << std::fixed << (Q - tot_q) << " ...";
            ///
            no_spat = true;
            h = 0;
            continue;
        }

        // Data la varaibile del problema più piccolo, ricavo le corrispondenze
        // con il problema più grande
        convert_size_x2(thevar.row, S1, S2, tupleori);
        convert_size_x2(thevar.col, S1, S2, tupledest);
        
        if(!no_spat)
        {
            // Risolve i quadrati 4x4 all'ottimo.
            for (NodeArcIdType k = 0; k < K; k++)
                ORai[k] = q_at_srcs[tupleori[k]], ORbj[k] = q_at_dsts[tupledest[k]];
            
            std::vector<TpCostType> thecost(K * K);
            for (NodeArcIdType i = 0; i < K; i++)
                for (NodeArcIdType j = 0; j < K; j++)
                    thecost[i * K + j] = twotpd_sptr->costs[tupleori[i] * N2 + tupledest[j]];
                    //thecost[i * F + j] = ((double)twoboard.cell[tupleori[i]].row - (double)twoboard.cell[tupledest[j]].row) * ((double)twoboard.cell[tupleori[i]].row - (double)twoboard.cell[tupledest[j]].row) + 1.0 * ((double)twoboard.cell[tupleori[i]].col - (double)twoboard.cell[tupledest[j]].col) * (twoboard.cell[tupleori[i]].col - twoboard.cell[tupledest[j]].col);
            
            NorthWestCorner<std::vector<double>, TSimplexData::tsimplex_dense_sol> nwc(thecost, ORai, ORbj);
            nwc.run();
            sol16 = nwc.getSolution();
            sol16arcs = nwc.getSolutionArcs();
        }
        else
        {
            sol16arcs.reset(new std::vector<NodeArcIdType>());
            for (NodeArcIdType i = 0; i < K; i++)
                for (NodeArcIdType j = 0; j < K; j++)
                    sol16arcs->push_back(i * K + j);
        }

        // Ricavo solo il costo totale,
        // bisognerebbe ricavare anche le variabili in base del problema più grande 
        // y[tupleori[i]][tupledest[j]] = var16[i][j] ma searebbe meglio farlo direttamente
        // nelle funzioni NorthWest16 e LeastCost16 per non perdere variabili di base con valore zero.        
        TpQuantityType arc_q;
        TpCostType arc_cost;
        NodeArcIdType i;
        NodeArcIdType j;
        
        for (NodeArcIdType e = 0; e < sol16arcs->size(); e++)
        {
            ///
            i = tupleori[(*sol16arcs)[e] / K];
            j = tupledest[(*sol16arcs)[e] % K];
            ///
            if(!nodes[i] || !nodes[M2 + j])
                continue;
            ///
            arc_q = no_spat ? std::min(q_at_srcs[i], q_at_dsts[j]) : sol16->get((*sol16arcs)[e]);
            ///
            q_at_srcs[i] -= arc_q;
            q_at_dsts[j] -= arc_q;            
            cost += (arc_cost = twotpd_sptr->costs[i * N2 + j]) * arc_q;
            tot_q += arc_q;
            ///
            sol_arcs_sptr->push_back(std::make_pair(i, j));
            ///
            #ifdef TESTDBG
            if(sol_sptr->contains(i * N2 + j))
                throw std::runtime_error("Arc " + std::to_string(i * N2 + j) + " already belong to solution");
            if(no_spat && arc_q > MYEPS)
                throw std::runtime_error("No SPAT and arc q. greater than 0");
            #endif
            ///    
            sol_sptr->set(i * N2 + j, arc_q > MYEPS
                                      ? arc_q
                                      : 
                                      #ifdef EPSQIS0
                                      0.0
                                      #else
                                      EPSQ
                                      #endif
                                      );
            ++added_vars;
            ///
            if(std::round(q_at_srcs[i]) < MYEPS && std::round(q_at_dsts[j]) < MYEPS)
                nodes[dsts_rem > srcs_rem ? i : M2 + j] = false;
            else
                nodes[std::round(q_at_srcs[i]) < MYEPS ? i : M2 + j] = false;
            
            nodes[i] ? ++dsts_rem : ++srcs_rem;
        }
        /// break if spat complete
        if(added_vars == M2 + N2 - 1)
            break;
        
        ++iter;
    }
    while(!stop);
    
    /// complete spanning tree if necessary
    if(added_vars < M2 + N2 - 1)
    {
        if(verbose)
        {
            FILE_LOG(logINFO) << "Matryoshka ::: Missing " << (M2 + N2 - 1 - added_vars) 
                              << " arcs to transport quantity " << std::setprecision(0) << std::fixed << (Q - tot_q) << " ...";
        }
        
        std::vector<NodeArcIdType> unedged_nodes;
        TpQuantityType arc_q;
        NodeArcIdType i;
        NodeArcIdType i_next = 0;
        NodeArcIdType i_last = 0;
        NodeArcIdType j;
        NodeArcIdType j_next = 0;
        //NodeArcIdType M2N2 = M2 * N2;
        
        for(NodeArcIdType e = 0; e < nodes.size(); e++)
            if(nodes[e])
            {
                if(!j_next && e >= M2)
                    i_last = j_next = unedged_nodes.size();
                unedged_nodes.push_back(e < M2 ? e : e - M2);
            }
        
        i_last--;
        while(added_vars < M2 + N2 - 1)
        {
            bool unedged = false;
            for(NodeArcIdType f = i_next; f <= i_last; f++)
            {
                for(NodeArcIdType e = j_next; e < unedged_nodes.size(); e++)
                {
                    ///
                    if(!nodes[i = unedged_nodes[f]] || !nodes[M2  + (j = unedged_nodes[e])])
                        continue;
                    ///
                    unedged = true;
                    arc_q = std::min(q_at_srcs[i], q_at_dsts[j]);
                    ///
                    q_at_srcs[i] -= arc_q;
                    q_at_dsts[j] -= arc_q;
                    cost += twotpd_sptr->costs[i * N2 + j] * arc_q;
                    tot_q += arc_q;
                    ///
                    sol_arcs_sptr->push_back(std::make_pair(i, j));
                    ///
                    #ifdef TESTDBG
                    if(sol_sptr->contains(i * N2 + j))
                        throw std::runtime_error("Arc " + std::to_string(i * N2 + j) + " already belong to solution");
                    if(arc_q > MYEPS)
                        throw std::runtime_error("No SPAT and arc q. greater than 0");
                    #endif
                    ///
                    sol_sptr->set(i * N2 + j, arc_q > MYEPS 
                                              ? arc_q 
                                              :
                                              #ifdef EPSQIS0
                                              0.0
                                              #else
                                              EPSQ
                                              #endif
                                              );
                    ++added_vars;
                    ///
                    if(std::round(q_at_srcs[i]) < MYEPS && std::round(q_at_dsts[j]) < MYEPS)
                        nodes[dsts_rem > srcs_rem ? i : M2 + j] = false;
                    else
                        nodes[std::round(q_at_srcs[i]) < MYEPS ? i : M2 + j] = false;
                    
                    nodes[i] ? ++dsts_rem : ++srcs_rem;
                    nodes[i] ? : ++i_next;
                    
                    /// break if spat complete
                    if(added_vars == M2 + N2 - 1)
                        break;
                }
            }
            
            if(!unedged && added_vars < M2 + N2 - 1)
                throw std::runtime_error("No way to find " + std::to_string(M2 + N2 - 1) + " arcs, " + std::to_string(M2 + N2 - 1 - added_vars) + " missing");
        }
        
        ++iter;
    }
    
    FILE_LOG(logINFO) << "Matryoshka ::: all " << added_vars << " arcs found ::: transhipped quantity " << std::setprecision(0) << std::fixed << tot_q;
    
    #ifdef TESTDBG
    checkv = 0.0;
    for(auto it = sol_arcs_sptr->begin(); it != sol_arcs_sptr->end(); it++)
        checkv += sol_sptr->get(it->first * N2 + it->second);
    FILE_LOG(logINFO) << "Check value = " << std::setprecision(0) << std::fixed << checkv 
                      << ", tot. q. " << tot_q
                      << ", added vars " << added_vars;
     #endif
    
    
    /// return value
    optresult optres;
    optres.obj_value = cost;
    optres.run_time = GETOPTT(start);
    optres.tags.resize(2);
    optres.tags[1] = std::string("MAT");
    optres.integer_values.resize(1);
    optres.integer_values[0] = added_vars;
    optres.double_values.resize(3, std::numeric_limits<double>::quiet_NaN());
    optres.double_values[0] = tot_q;
    optres.double_values[2] = GETOPTTMS(start);

    //FILE_LOG(logINFO)  << "Matryoshka (MAT) method t cost = " << std::setprecision(1) << std::fixed << optres.obj_value << ", shipped q = " << tot_q
    //                   << ", opt time = " << std::setprecision(3) << std::fixed << optres.run_time << " [s], " << iter << " iterations"
    //                   << ", added vars = " << added_vars;

    return optres;
}
