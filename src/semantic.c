#include "../include/semantic.h"
#include "../include/symboltable.h"
#include "../include/log.h"

void semantic_analysis(Node *ast){
    Scope *scope = create_global_scope();
    log_warn("SEMANTIC ANALYSIS NOT IMPLEMENTED");


    exit_scope(scope);
}
