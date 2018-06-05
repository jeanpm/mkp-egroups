library(xtable)

nevals = "100000000l"
lr = 0.01
t  = 0

options(scipen=999, width=10000)
columns = c("time.s", "core.size", "lp.sol", "best.known", "best.found", "gap", "gap%", "nevals")

cnames = c("Instance", "Best", rep(c("$Sw_{d=1}$", "$Sw_{d=2}$", "$Sh_{d=1}$", "$Sh_{d=2}$", "$CBGA$"),2))

bold_the_min = function(x, isoptimal)
{
    min = min(x)
    for (i in 1:length(x))
    {
        if (x[i] == min)
            if (length(isoptimal) > 0)
              x[i] = paste(isoptimal[i], "\\", "textbf{", x[i], "}", sep='')
            else
                x[i] = paste("\\", "textbf{", x[i], "}", sep='')
    }   
    return(x)
}


write_results = function(dir, instance_name, INST, RGop, DCases) 
{
    table = NULL
    total_gap = NULL
    total_time = NULL
    
    fileprefix = paste(dir, instance_name, sep='')
    for (idx in INST)
    {
        idx = sprintf("%02d", idx)
        
        instance = paste(instance_name, idx, sep="-")

        row_gap  = NULL
        row_time = NULL
        
        best_known = 0
        
        target_reached = NULL

        for (RG_operator in RGop)
        {
            for (DC in DCases) 
            {
                if (RG_operator == 3 && DC == 2)
                    break
                    
                instance_file = paste(fileprefix, idx, RG_operator, DC, 0, t, lr, nevals, sep="-")

                exp = read.table(paste(instance_file, ".out", sep=''))
                best_known = exp[1, 4]
                best_found = max(exp[,5])
                
                gap = mean(exp[, 6])  # Average best and time
                time = mean(exp[, 1])  # Average best and time
                
                gap  = round(gap, 2)
                time = round(time, 2)
                
                if (best_found == best_known) 
                    target_reached = cbind(target_reached,"$^\\checkmark$")
                else
                    target_reached = cbind(target_reached,"")
                
                row_gap = cbind(row_gap, gap)         # Max found
                row_time = cbind(row_time, time)
            }
        }
        
        total_gap = rbind(total_gap, row_gap)
        total_time = rbind(total_time, row_time)
        
        row_gap  = bold_the_min(row_gap, target_reached)
        row_time = bold_the_min(row_time, NULL)
        
        row = cbind(instance, best_known, row_gap, row_time)
        table = rbind(table, row)
    }
    
    # Add a row for the averages
    instance = paste(instance_name, "*", sep="-")
    total_gap  = apply(total_gap, 2, sum)
    total_time = apply(total_time, 2, sum)
    
    total_gap = round(total_gap, 2)
    total_time = round(total_time, 2)
    
    total_gap = bold_the_min(total_gap, NULL)
    total_time = bold_the_min(total_time, NULL)

    row = c(instance, "-", total_gap, total_time)
    table = rbind(table, row)
    
    return(table)
}

cat("Example:\twrite_results(dir, instance_name, INST, RGopMut, DCases)\n");
cat("Example:\twrite_results('output/', '10-500', 0:10, c(0,1), c(1,2))\n");

instance_names = c('5-100', '10-100',  '5-250', '10-250',  '5-500', '10-500')

for (i in instance_names)
{
    tab = write_results('output/', i, 0:29, c(0,1,3), c(1,2))
    rownames(tab) = NULL
    
    colnames(tab) = cnames

    xt = xtable(tab)
    align(xt) <- "l|lr|rrrrr|rrrrr|" 
    
    addtorow <- list()
    addtorow$pos <- list(-1)
    addtorow$command <- paste0(paste0('\\cline{3-12}\\multicolumn{2}{c}{}& \\multicolumn{5}{|c|}{Gap} & \\multicolumn{5}{|c|}{Time(s)}', collapse=''), '\\\\')

    xtable_file = paste('article/xtables/', i, ".xtable", sep='')
    
    cat("Writing results for ", xtable_file, ".\n");    
    print(xt, include.rownames=FALSE, size='small',  add.to.row=addtorow,
        sanitize.text.function = function(x){x}, booktabs=TRUE, 
        file=xtable_file)
}





