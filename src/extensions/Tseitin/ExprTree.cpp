#include "ExprTree.hh"
#include <iostream>
#include "Parser.hh"


// déclaration des variables globales
unsigned ExprTree::lastUsedId;
std::vector<std::pair<std::string,unsigned> > ExprTree::varNumbers;





static inline void
addClause(std::vector<clause>& cnf, const literal& l1)
{
    clause tmp;
    tmp.push_back(l1);
    cnf.push_back(tmp);
}
static inline void
addClause(std::vector<clause>& cnf, const literal& l1, const literal& l2)
{
    clause tmp;
    tmp.push_back(l1);
    tmp.push_back(l2);
    cnf.push_back(tmp);
}
static inline void
addClause(std::vector<clause>& cnf, const literal& l1, const literal& l2, const literal& l3)
{
    clause tmp;
    tmp.push_back(l1);
    tmp.push_back(l2);
    tmp.push_back(l3);
    cnf.push_back(tmp);
}





unsigned ValExpr::getVarId() const
{
    unsigned id;
    // cherche si la variable à déjà un numéro attribué
    // et lui attribue un numéro si non
    std::vector<std::pair<std::string,unsigned> >::const_iterator it;
    for(it = ExprTree::varNumbers.begin(); it != ExprTree::varNumbers.end(); ++it)
    {
        if (it->first == _name)
            break;
    }
    if (it == ExprTree::varNumbers.end()) {
        id = ++ ExprTree::lastUsedId;
        ExprTree::varNumbers.push_back(std::pair<std::string,unsigned>(_name,id));
    } else {
        id = it->second;
    }
    return id;
}





void NodeExpr::inversion()
{
    if (_type == ExprTree::XOR)
    {
        _args.front()->inversion();
    }
    else
    {
        _type = (_type == ExprTree::AND) ? ExprTree::OR : ExprTree::AND;
        for(fils::iterator it = _args.begin(); it != _args.end(); ++it)
            (*it)->inversion();
    }
}
void ValExpr::inversion()
{
    _val = ! _val;
}



void NodeExpr::print(std::ostream& out) const
{
    const char* op = (_type == ExprTree::AND ? " /\\ " :
                     (_type == ExprTree::XOR ? " XOR " : " \\/ "));
    out << "(" << _args.front();
    for(fils::const_iterator it = _args.begin()+1; it != _args.end(); ++it)
        out << op << *it;
    out << ")";
}
void ValExpr::print(std::ostream& out) const
{
    out << (_val ? '~' : ' ') << _name;
}



void NodeExpr::addCNF(std::vector<clause>& cnf) const
{
    if (_type == ExprTree::AND)
    {
        for(fils::const_iterator it = _args.begin(); it != _args.end(); ++it)
            (*it)->addCNF(cnf);
    }
    else
    {
        addClause(cnf, this->getLiteral(cnf));
    }
}
void ValExpr::addCNF(std::vector<clause>& cnf) const
{
    addClause(cnf, this->getLiteral(cnf));
}



void NodeExpr::simplify()
{
    for(fils::const_iterator it = _args.begin(); it != _args.end(); ++it)
    {
        if ((*it)->remonte(_args, _type))
            delete *it;
    }
}
void ValExpr::simplify()
{
}



bool NodeExpr::remonte(std::vector<ExprTree*>& dest, ExprTree::type t)
{
    const bool isSameType = _type == t;
    if (isSameType)
    {
        for(fils::iterator it = _args.begin(); it != _args.end(); it++)
        {
            // si le fils est de type t, on fait remonter ses enfants
            // et on détruit le noeuds correspondant (qui est maintenant vide)
            if((*it)->remonte(dest,t))
                delete *it;
            // sinon on fait remonter ce fils
            else
                dest.push_back(*it);
        }
        // ne pas oublier de vider _args : le noeud n'a plus de fils (ils sont tous remonté dans dest)
        _args.clear();
    }
    return isSameType;
}
bool ValExpr::remonte(std::vector<ExprTree*>& dest, ExprTree::type t)
{
    (void)dest; // évite de générer un 'warning : unused parameter' à la compilation
    (void)t;    // évite de générer un 'warning : unused parameter' à la compilation
    return false;
}



literal NodeExpr::getLiteral(std::vector<clause>& cnf) const
{
    // récupère les litéraux correspondant aux fils
    std::vector<literal> litFils;
    litFils.reserve(_args.size());
    for(fils::const_iterator it = _args.begin(); it != _args.end(); ++it)
        litFils.push_back((*it)->getLiteral(cnf));
    
    // litéral correspondant au noeud
    literal self;

    // applique la transformation de Tseitin correspondant au type de noeud

    // pour le XOR, on traite le noeud comme s'il était binaire :
    // ça simplifie les expressions, sans rajouter enormement de variable
    if(_type == ExprTree::XOR)
    {
        while (litFils.size() >= 2)
        {
            // fait correspondre self à fils1 xor fils2
            self = literal(++lastUsedId, true);
            literal fils1 = litFils[litFils.size()-1];
            literal fils2 = litFils[litFils.size()-2];

            // on a : s = a xor b  ssi ~(s xor a xor b)
            // ie ssi ~(s+a+b impaire) ie ssi (~s | a | b) & (s | ~a | b) & (a | a | ~b) & (~s | ~a | ~b)
            addClause(cnf, invert(self), fils1, fils2);
            addClause(cnf, self, invert(fils1), fils2);
            addClause(cnf, self, fils1, invert(fils2));
            addClause(cnf, invert(self), invert(fils1), invert(fils2));

            // remplace fils1 et fils2 par ce litéral dans le xor qu'on est en train de calculer
            litFils.pop_back();
            litFils.pop_back();
            litFils.push_back(self);
        }
    }
    // pour and et or, les expressions sont simple même avec une arité > 2
    else if(_type == ExprTree::AND)
    {
        /*
        while (litFils.size() >= 2)
        {
            // fait correspondre self à fils1 and fils2
            self = literal(++lastUsedId, true);
            literal fils1 = litFils[litFils.size()-1];
            literal fils2 = litFils[litFils.size()-2];

            addClause(cnf, invert(self), fils1);
            addClause(cnf, invert(self), fils2);
            addClause(cnf, self, invert(fils1), invert(fils2));

            // remplace fils1 et fils2 par ce litéral dans le and qu'on est en train de calculer
            litFils.pop_back();
            litFils.pop_back();
            litFils.push_back(self);
        }
        /*/
        self = literal(++lastUsedId, true);

        // ajoute les (non(self), args[i])
        for(std::vector<literal>::iterator it = litFils.begin(); it != litFils.end(); ++it)
            addClause(cnf, invert(self), *it);
        
        // ajoute (self, non(args[0]), ... non(args[k]))
        for(std::vector<literal>::iterator it = litFils.begin(); it != litFils.end(); ++it)
            it->second = ! it->second;
        litFils.push_back(self);
        cnf.push_back(litFils);
    }
    else if(ExprTree::OR)
    {
        /*
        while (litFils.size() >= 2)
        {
            // fait correspondre self à fils1 or fils2
            self = literal(++lastUsedId, true);
            literal fils1 = litFils[litFils.size()-1];
            literal fils2 = litFils[litFils.size()-2];

            addClause(cnf, self, invert(fils1));
            addClause(cnf, self, invert(fils2));
            addClause(cnf, invert(self), fils1, fils2);

            // remplace fils1 et fils2 par ce litéral dans le or qu'on est en train de calculer
            litFils.pop_back();
            litFils.pop_back();
            litFils.push_back(self);
        }
        /*/
        self = literal(++lastUsedId, true);

        // ajoute les (self, non(args[i]))
        for(std::vector<literal>::iterator it = litFils.begin(); it != litFils.end(); ++it)
            addClause(cnf, self, invert(*it));
        
        // ajoute (non(self), args[0], ... args[k])
        litFils.push_back(invert(self));
        cnf.push_back(litFils);
    }
    return self;
}
literal ValExpr::getLiteral(std::vector<clause>& cnf) const
{
    (void)cnf; // évite de générer un 'warning : unused parameter' à la compilation
    return literal(getVarId(), _val);
}




