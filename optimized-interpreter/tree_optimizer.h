#ifndef TREE_OPTIMIZER_H
#define TREE_OPTIMIZER_H

#include <vector>

#include "lexer.h"
#include "parser.h"


class TreeOptimizer
{
public:
    virtual ~TreeOptimizer() {}
    bool optimize(ParserTree* tree);

protected:
    TreeOptimizer(ParserTree::Type type)
        : type(type) {}
    ParserTree::Type type;
    // bool wasOptimized = false;
    // int currentIndex = -1;
    virtual bool _optimize(ParserTree* parent) = 0;
};


class OptimizerExpression : public TreeOptimizer
{
public:
    OptimizerExpression();
    ~OptimizerExpression() {}

protected:
    bool _optimize(ParserTree* parent) override;
};

// class OptimizerBlock : public TreeOptimizer
// {
// public:
//     OptimizerBlock();

// protected:
//     bool _optimize(ParserTree* parent) override;
// };

class OptimizerIf : public TreeOptimizer
{
public:
    OptimizerIf();
    ~OptimizerIf() {}

protected:
    bool _optimize(ParserTree* parent) override;
};

class OptimizerWhile : public TreeOptimizer
{
public:
    OptimizerWhile();
    ~OptimizerWhile() {}

protected:
    bool _optimize(ParserTree* parent) override;
};

class OptimizerDoWhile : public TreeOptimizer
{
public:
    OptimizerDoWhile();
    ~OptimizerDoWhile() {}

protected:
    bool _optimize(ParserTree* parent) override;
};



#endif  // TREE_OPTIMIZER_H
