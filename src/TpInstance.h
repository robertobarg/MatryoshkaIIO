///
/// src/instance/TpInstance.h
///
/// Written by Roberto Bargetto
/// 	   DIGEP
/// 	   Politecnico di Torino
/// 	   Corso Duca degli Abruzzi, 10129, Torino
/// 	   Italy
///
/// Copyright 2023 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
///
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///

#ifndef TPINST_H
#define TPINST_H

#include <string>
#include <memory>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>

#define F4DDR 1e3

//SGN_NODEARCID_TYPE

#define NODEARC_INT_TYPE long
typedef NODEARC_INT_TYPE NodeArcIdTypeSGND;
typedef unsigned NODEARC_INT_TYPE NodeArcIdType;

typedef double TpCostType;
typedef double TpQuantityType;

class TpInstance
{
public:
    class euclidean_dist
    {
    public:
        euclidean_dist(unsigned int fc = 0)
        {
            if(fc == 1)
            {
                distance_fptr = &euclidean_dist::esqrtd;
            }
            else if(fc == 2)
            {
                distance_fptr = &euclidean_dist::esqd;
            }
            else if(fc == 3)
            {
                distance_fptr = &euclidean_dist::eabsd;
            }
            else if(fc == 4)
            {
                distance_fptr = &euclidean_dist::emaxabsd;
            }
            else
            {
                distance_fptr = &euclidean_dist::esqd;
            }
        }
        euclidean_dist(NodeArcIdType aN, unsigned int fc = 0)
            : N(aN)
        {
            S = static_cast<NodeArcIdType>(std::sqrt(N));
            NN = N * N;

            if(fc == 1)
            {
                distance_fptr = &euclidean_dist::esqrtd;
            }
            else if(fc == 2)
            {
                distance_fptr = &euclidean_dist::esqd;
            }
            else if(fc == 3)
            {
                distance_fptr = &euclidean_dist::eabsd;
            }
            else if(fc == 4)
            {
                distance_fptr = &euclidean_dist::emaxabsd;
            }
            else
            {
                distance_fptr = &euclidean_dist::esqd;
            }
        }
        ~euclidean_dist() { }
        
        inline NodeArcIdType size()
        {
            return NN;
        }
        
        inline void setSrcDstMapPtrs(std::vector<NodeArcIdType>* srs_map_ptr, std::vector<NodeArcIdType>* dsts_map_ptr)
        {
            sources_map_ptr = srs_map_ptr;
            destinations_map_ptr = dsts_map_ptr;
            Nredi = destinations_map_ptr->size();
        }
        
        inline double operator[](NodeArcIdType e) const
        {
            NodeArcIdType i = sources_map_ptr == nullptr ? e / N : (*sources_map_ptr)[e / Nredi];
            NodeArcIdType j = destinations_map_ptr == nullptr ? e % N : (*destinations_map_ptr)[e % Nredi];

            return (this->*distance_fptr)(i, j);
        }
        
        inline double getMax() const
        {
            return (this->*distance_fptr)(0, N);
        }
        
    private:
    
        double (euclidean_dist::*distance_fptr)(NodeArcIdType i, NodeArcIdType j) const = nullptr;
        
        inline std::pair<NodeArcIdType, NodeArcIdType> getXY(NodeArcIdType i)
        {
            return std::make_pair(i / S, i % S);
        }
        
        inline double esqd(NodeArcIdType i, NodeArcIdType j) const
        {
            NodeArcIdTypeSGND x, y, w, z;
            x = i / S;
            y = i % S;
            w = j / S;
            z = j % S;
            
            return std::pow(x - w, 2.0) + std::pow(y - z, 2.0);
        }
        
        inline double esqrtd(NodeArcIdType i, NodeArcIdType j) const
        {
            NodeArcIdTypeSGND x, y, w, z;
            x = i / S;
            y = i % S;
            w = j / S;
            z = j % S;
            
            return std::round(F4DDR * std::pow(std::pow(x - w, 2.0) + std::pow(y - z, 2.0), 0.5));
        }
        
        inline double eabsd(NodeArcIdType i, NodeArcIdType j) const
        {
            NodeArcIdTypeSGND x, y, w, z;
            x = i / S;
            y = i % S;
            w = j / S;
            z = j % S;
            
            return std::abs(x - w) + std::abs(y - z);
        }
        
        inline double emaxabsd(NodeArcIdType i, NodeArcIdType j) const
        {
            NodeArcIdTypeSGND x, y, w, z;
            x = i / S;
            y = i % S;
            w = j / S;
            z = j % S;
            
            return std::max(std::abs(x - w), std::abs(y - z));
        }
        
        NodeArcIdType N;
        NodeArcIdType Nredi;
        NodeArcIdType S;
        NodeArcIdType NN;

        std::vector<NodeArcIdType>* sources_map_ptr = nullptr;
        std::vector<NodeArcIdType>* destinations_map_ptr = nullptr;
    };
    #ifdef EDOTF
    typedef class euclidean_dist tcosts;
    #else
    typedef std::vector<TpCostType> tcosts;
    #endif

    struct TProblemData
    {
        unsigned long long rndseed;
        unsigned int cost_f = 0;
        bool reduced = false;
        bool swapped = false;
        
        TpQuantityType tot_dst_quantity = std::numeric_limits<TpQuantityType>::quiet_NaN();
        TpQuantityType tot_src_quantity = std::numeric_limits<TpQuantityType>::quiet_NaN();

        double minc = std::numeric_limits<double>::quiet_NaN();
        double maxc = std::numeric_limits<double>::quiet_NaN();
        double avgc = std::numeric_limits<double>::quiet_NaN();
        
        NodeArcIdType m;
        NodeArcIdType n;
        
        std::vector<TpQuantityType> sources;
        std::vector<TpQuantityType> destinations;
        tcosts costs;
        std::vector<std::pair<NodeArcIdType, double>> pvars;
        
        NodeArcIdType m_orgn;
        NodeArcIdType n_orgn;
        NodeArcIdType srcs_at_zero;
        NodeArcIdType dsts_at_zero;
        std::vector<NodeArcIdType> sources_map;
        std::vector<NodeArcIdType> destinations_map;
        std::vector<NodeArcIdType> sources_map_bw;
        std::vector<NodeArcIdType> destinations_map_bw;
        
        std::string name = "";
        std::string initial_base_fname = "current_base.txt";
        std::string solution_output_fname = "current_sol.txt";
        bool dump_down_sol = false;
        unsigned long long max_macroiter = (std::numeric_limits<unsigned long long>::max)();
        
        /// method to remove srcs and dsts with 0 q.
        bool reduce();
        bool hasSrcOrDstAt0();
        void logStrictInfo();
    };
    
    typedef struct TProblemData* TProblemDataPtr;
    typedef struct TProblemData& TProblemDataRef;
        
    TpInstance(std::string);
    ~TpInstance();

    std::string getBaseName();
    std::string getName();

    bool isToGenerate();
    bool isToWrite();
    void toGenerate();
    void toWrite();
    void logInfo();
    void setName(std::string);
    void reset(const std::shared_ptr<TProblemData>&);
    void write2file(std::string);
    
    std::shared_ptr<std::vector<double>> getProvidedBasis();
    
    
    const std::shared_ptr<TProblemData>& getInstanceData();
    std::shared_ptr<TProblemData> generateData();
    std::shared_ptr<TProblemData> generateData(bool, unsigned int, unsigned int, double, double, NodeArcIdType, NodeArcIdType, unsigned long long, unsigned int iid = 1);

private:
    std::string ifname;
    bool to_generate = false;
    bool to_write_to_file = false;

    std::shared_ptr<TProblemData> tp_data_sptr;
    std::shared_ptr<std::vector<double>> basis_sptr;
};

#endif // TPINST_H