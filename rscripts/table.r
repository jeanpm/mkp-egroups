
file = "output/10-100"
nevals = "100000000l"
lr = 0.01
t  = 0

options(scipen=999)
columns = c("RG-swap_1", "RG-swap_2", "RG-shuffle_1", "RG-shuffle_2", "CBGA_1", "CBGA_2")

table = NULL

for (idx in INST)
{
    idx = sprintf("%02d", idx)

    row = rep(0, 54)
    i = 2 # Indice para a linha
    for (RG_operator in c(0,1,3)) {
        for (DC in c(1,2)) {
            
                 
                
            instance = paste(file, idx, RG_operator, DC, 0, t, lr, nevals, sep="-")

            exp = read.table(paste(instance, ".out", sep=''))
            row[1]   = exp[1,4]           # Best known
            row[i]   = max(exp[, 5])      # Max found
            row[i+1] = mean(exp[,5])      # Avg Best found
            row[i+2] = sd(exp[,5])        # Std. Dev. found
            row[i+3] = mean(exp[,1])      # Avg time (s)
            row[i+4] = sd(exp[,1])        # Std. Dev. time (s)
            row[i+5] = mean(exp[,6])      # Avg GAP
            row[i+6] = sd(exp[,6])        # Std. Dev. GAP
            row[i+7] = mean(exp[,8])      # Avg Evals
            row[i+8] = sd(exp[,8])        # Std. Dev Evals
            
            i = i + 10
        }
    }
    table = rbind(table, row)
}

table = round(table,2)

COLS = c(2,3,4,5,6,7,8,9)

#print(table[1])

ptable = NULL

for (i in 0:5)
    ptable = rbind(ptable, table[COLS + i*10])

colnames(ptable) = c("max", "avg.Best", "std.best", "avg.time", "std.time", "avg.gap", "std.gap", "avg.evals")
#print(ptable)

#OBJVALS = c(2,7,8)
#print(table[c(OBJVALS, OBJVALS + 10, OBJVALS + 20, OBJVALS + 30)])
