copy
(
    select
        case when algorithm = 'rd' then 'matr-iio4d' else 'iio4d' end,
        objftype as oftype,
        imgsize2::text as imgsize,
        classnm as imgclass,
        --- inr as instance,
        regexp_replace(instance::text, '\d{2}(\d{2})\d{2}(\d{2})', E'\\1-\\2') as instance,
        -- opt objf        
        objvalue as opt_objf,
        -- tot opt time
        round(doublev9 / 1000.0, 3) as optsoltm_sec,
        -- init sol time
        round(doublev1 / 1000.0, 3) as initsoltm_sec,
        -- iterations
        intv5 as step2basischanges,
        intv4 as iterations
    from :tab as tb
    order by algorithm, objftype, imgsize2, classnm, instance
    
) 
to stdout with delimiter ',' header csv;

