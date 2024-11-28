#include "Manager.h"

#include <algorithm>


namespace ClassProject {
    Manager::Manager() {
        init_unique_tb();
    }

    void Manager::init_unique_tb() {
        computed_tb[uTableRow {False(), False(), False(), "false"}] = False();
        unique_tb.emplace(uniqueTableSize(), uTableRow {False(), False(), False(), "false"});
        computed_tb[uTableRow{True(), True(), True(), "true"}] = True();
        unique_tb.emplace(uniqueTableSize(), uTableRow{True(), True(), True(), "true"});
    }

    BDD_ID Manager::createVar(const std::string &label){
        const BDD_ID id = get_nextID();
        unique_tb.emplace(uniqueTableSize(), uTableRow{True(), False(), id, label});
        return id;
    }

    const BDD_ID &Manager::True(){
        return TrueId;
    }

    const BDD_ID &Manager::False(){
        return FalseId;
    }

    bool Manager::isConstant(const BDD_ID f){
        return f <= 1;
    }

    bool Manager::isVariable(const BDD_ID x){
        return unique_tb.at(x).topVar == x && !isConstant(x);
    }

    BDD_ID Manager::topVar(const BDD_ID f){
        return unique_tb.at(f).topVar;
    }

    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e){
        // Check for terminal cases
        if (isConstant(i)) {
           return i == True() ? t : e;
        }
        if (t == TrueId && e == FalseId) {
            return i;
        }
        if (t == e)
        {
            return t;
        }
        // find the smallest top index for x
        BDD_ID x = topVar(i);
        if (topVar(t) < x && isVariable(topVar(t)))
        {
            x = topVar(t);
        } else if (topVar(e) < x && isVariable(topVar(e))) {
            x = topVar(e);
        }

        // calculate r_high and r_low like Slide 2-17 VDS Lecture
        const BDD_ID high = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
        const BDD_ID low = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));
        if (high == low)
        {
            return high;
        }

        // Check for entry already existing entry in computed table (dummy is not used for find)
        // Entry not found
        if (const auto comp_tb_entry = computed_tb.find(uTableRow(high, low, x)); comp_tb_entry == computed_tb.end())
        {
            // Add Entry
            const BDD_ID new_id = get_nextID();
            computed_tb[uTableRow(high, low, x)] = new_id;
            // Generate Label for Visualization
            const auto label = "if" + unique_tb.at(x).label + " then " + unique_tb.at(high).label + " else " + unique_tb.at(low).label;
            unique_tb.emplace(new_id, uTableRow(high, low, x, label));
            return get_nextID();
        // Entry found -> return result
        } else {
            return comp_tb_entry->second;
        }
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
        // Check for terminal Case and relevancy of x
        if (isConstant(f) || topVar(f) > x) {
            return f;
        }

        // CoFactor of f w.r.t x is high path
        if (topVar(f) == x) {
            return unique_tb.at(f).high;
        }

        //recursiv high and low
        BDD_ID high = coFactorTrue(unique_tb.at(f).high, x);
        BDD_ID low = coFactorTrue(unique_tb.at(f).low, x);
        // compute result with ite-function
        return ite(topVar(f), high, low);
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x){
        // Check for terminal Case and relevancy of x
        if (isConstant(f) || topVar(f) > x) {
            return f;
        }

        // CoFactor of f w.r.t x is low path
        if (topVar(f) == x) {
            return unique_tb.at(f).high;
        }

        //recursiv high and low
        BDD_ID high = coFactorFalse(unique_tb.at(f).high, x);
        BDD_ID low = coFactorFalse(unique_tb.at(f).low, x);
        // compute result with ite-function
        return ite(topVar(f), high, low);
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f){
        return unique_tb.at(f).high;
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f){
        return unique_tb.at(f).low;    
    }

    // Slide 2-15
    BDD_ID Manager::and2(BDD_ID a, BDD_ID b){
        return ite(a, b, False());
    }

    // Slide 2-15
    BDD_ID Manager::or2(BDD_ID a, BDD_ID b){
        return ite(a, True(), b);
    }

    // Slide 2-15
    BDD_ID Manager::xor2(BDD_ID a, BDD_ID b){
        return ite(a, neg(b), b);
    }

    // Slide 2-15
    BDD_ID Manager::neg(BDD_ID a){
        return ite(a, False(), True());
    }

    // Abb. 4 https://agra.informatik.uni-bremen.de/doc/software/manual/index.html
    BDD_ID Manager::nand2(BDD_ID a, BDD_ID b){
        return ite(a, neg(b), True());
    }

    // Abb. 4 https://agra.informatik.uni-bremen.de/doc/software/manual/index.html
    BDD_ID Manager::nor2(BDD_ID a, BDD_ID b){
        return ite(a, False(), neg(b));
    }

    // Abb. 4 https://agra.informatik.uni-bremen.de/doc/software/manual/index.html
    BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b){
        return ite(a, b, neg(b));
    }

    std::string Manager::getTopVarName(const BDD_ID &root){
        return unique_tb.at(root).label;
    }

    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root){
        nodes_of_root.emplace(root);
        if (root > TrueId){
            findNodes(coFactorTrue(root), nodes_of_root);
            findNodes(coFactorFalse(root), nodes_of_root);
        }
    }

    void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root){

    }

    size_t Manager::uniqueTableSize(){
        return unique_tb.size();
    }

    void Manager::visualizeBDD(std::string filepath, BDD_ID &root){

    }
    
}

