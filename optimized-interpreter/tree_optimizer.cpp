#include "tree_optimizer.h"

#include <iostream>
#include <cassert>

#include <list>

bool TreeOptimizer::optimize(ParserTree *tree)
{
    bool wasOptimized = false;
    std::list<ParserTree*> queue;
    queue.push_back(tree);
    while (queue.size()) {
        ParserTree* t = queue.front();
        queue.pop_front();
        for (int i = 0; i < (int)t->units.size(); i++) {
            Unit &u = t->units[i];
            if (u.isTree() &&
                u.getTree()->type == type) {
                if (_optimize(t)) {
                    wasOptimized = true;
                }
                break;
            }
        }
        for (int i = 0; i < (int)t->units.size(); i++) {
            Unit &u = t->units[i];
            if (u.isTree()) {
                queue.push_back(u.getTree());
            }
        }
    }
    return wasOptimized;
}


OptimizerExpression::OptimizerExpression()
    : TreeOptimizer(ParserTree::expression)
{}

bool OptimizerExpression::_optimize(ParserTree* parent)
{
    // expression удаляем, если units.size() == 0
    bool wasOptimized = false;
    for (int i = 0; i < (int)parent->units.size(); i++) {
        if (parent->units[i].isTree() &&
            parent->units[i].getTree()->type == type) {
            ParserTree* e = parent->units[i].getTree();
            if (e->units.size() == 0) {
                parent->units.erase(parent->units.begin() + i);
                i--;
                wasOptimized = true;
            }
        }
    }
    return wasOptimized;
}

// OptimizerBlock::OptimizerBlock()
//     : TreeOptimizer(ParserTree::block)
// {}

// bool OptimizerBlock::_optimize(ParserTree* parent)
// {
//     // block удаляем, если units.size() == 0,
//     // или units[1].isToken() и units.size() == 2
//     bool wasOptimized = false;
//     for (int i = 0; i < (int)parent->units.size(); i++) {
//         if (parent->units[i].isTree() &&
//             parent->units[i].getTree()->type == type) {
//             ParserTree* e = parent->units[i].getTree();
//             if (e->units.size() == 0) {
//                 parent->units.erase(parent->units.begin() + i);
//                 delete e;
//                 i--;
//                 wasOptimized = true;
//             }
//             else if (e->units[0].isToken()) {
//                 parent->units.erase(parent->units.begin() + i);
//                 delete e;
//                 i--;
//                 wasOptimized = true;
//             }
//         }
//     }
//     return wasOptimized;
// }

bool isBlockEmpty(ParserTree* t)
{
    // Проверяет, что t - блок с units.size() == 0
    //, или что он состоит из выражений, которые задают пустые блоки
    if (t->units.size() == 1) {
        ParserTree* e = t->units[0].getTree();
        if (e->units[0].getTree()->type == ParserTree::block) {
            return isBlockEmpty(e->units[0].getTree());
        }
    }
    else {
        for (int i = 1; i < (int)t->units.size()-1; i++) {
            ParserTree* e = t->units[i].getTree();
            if (e->units[0].getTree()->type == ParserTree::block && 
                isBlockEmpty(e->units[0].getTree())) {}
            else {
                return false;
            }
        }
    }
    return true;
}

OptimizerIf::OptimizerIf()
    : TreeOptimizer(ParserTree::if_else)
{}

bool OptimizerIf::_optimize(ParserTree* parent)
{
    // if_else удаляем, если units.size() == 5 и
    //     isBlockEmpty(units[2]) && isBlockEmpty(units[4])
    // или units.size() == 3 и isBlockEmpty(units[2])
    bool wasOptimized = false;
    for (int i = 0; i < (int)parent->units.size(); i++) {
        if (parent->units[i].isTree() &&
            parent->units[i].getTree()->type == type) {
            ParserTree* e = parent->units[i].getTree();
            if (e->units.size() == 3) {
                if (isBlockEmpty(e->units[2].getTree())) {
                    parent->units.erase(parent->units.begin() + i);
                    i--;
                    wasOptimized = true;
                }
            }
            else {
                bool elseEmpty = isBlockEmpty(e->units[4].getTree());
                if (isBlockEmpty(e->units[2].getTree()) && elseEmpty) {
                    parent->units.erase(parent->units.begin() + i);
                    i--;
                    wasOptimized = true;
                }
                else if (elseEmpty) {
                    // удаление else
                    e->units.erase(e->units.begin() + 3, e->units.end());
                }
            }
        }
    }
    return wasOptimized;
}


OptimizerWhile::OptimizerWhile()
    : TreeOptimizer(ParserTree::while_block)
{}

bool OptimizerWhile::_optimize(ParserTree* parent)
{
    // while_block удаляем, если isBlockEmpty(units[2])
    bool wasOptimized = false;
    for (int i = 0; i < (int)parent->units.size(); i++) {
        if (parent->units[i].isTree() &&
            parent->units[i].getTree()->type == type) {
            ParserTree* e = parent->units[i].getTree();
            if (isBlockEmpty(e->units[2].getTree())) {
                parent->units.erase(parent->units.begin() + i);
                i--;
                wasOptimized = true;
            }
        }
    }
    return wasOptimized;
}

OptimizerDoWhile::OptimizerDoWhile()
    : TreeOptimizer(ParserTree::do_while)
{}

bool OptimizerDoWhile::_optimize(ParserTree* parent)
{
    // do_while удаляем, если isBlockEmpty(units[1])
    bool wasOptimized = false;
    for (int i = 0; i < (int)parent->units.size(); i++) {
        if (parent->units[i].isTree() &&
            parent->units[i].getTree()->type == type) {
            ParserTree* e = parent->units[i].getTree();
            if (isBlockEmpty(e->units[1].getTree())) {
                parent->units.erase(parent->units.begin() + i);
                i--;
                wasOptimized = true;
            }
        }
    }
    return wasOptimized;
}

