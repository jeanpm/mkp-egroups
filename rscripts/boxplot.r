library(extrafont)
loadfonts(quiet=TRUE)

nevals = "100000000l"
lr = 0.01
t  = 0

options(scipen=999)
columns = c("time.s", "core.size", "lp.sol", "best.known", "best.found", "gap", "gap%", "nevals")



plot_instance = function(fileprefix, INST, RGop, DCases, coluna) 
{
    table = NULL
    for (idx in INST)
    {
        idx = sprintf("%02d", idx)

        row = rep(0, 54)

        for (RG_operator in RGop)
        {
            for (DC in DCases) 
            {
                instance = paste(fileprefix, idx, RG_operator, DC, 0, t, lr, nevals, sep="-")

                exp = read.table(paste(instance, ".out", sep=''))
                table = cbind(table, exp[, coluna])      # Max found
            }
        }
    }
    boxplot(table, las=1, main=columns[coluna])#, outline=FALSE)
}

cat("Example:\tplot_instance(fileprefix, INSTances, RGopMutation, DCases, coluna)\n");
cat("Example:\tplot_instance('output/10-500', 0:10, c(0,1), c(1,2), 6)\n");
