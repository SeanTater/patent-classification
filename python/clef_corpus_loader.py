
# coding: utf-8

## CLEF-IP2010 Corpus Loader

####### Load clef-ip2010 patents corpus from DB table

# In[ ]:

def load_corpus(vocabulary_src,train_or_test):
    import sqlite3 as sqlitedb
    from clef_globals import *

    if train_or_test not in train_or_test_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(train_or_test,train_or_test_values))
    if vocabulary_src not in vocabulary_src_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(vocabulary_src,vocabulary_src_values))
        
    if train_or_test=='both':
        patents_query = 'select p.id,lower({0}) from patents p,patent_{0}_language_train_test_vw pl where p.id=pl.id and language=\'en\' and {0}!=\'\' and description!=\'\' and substr(p.id,length(p.id)-1) in (\'A1\',\'A2\') group by substr(p.id,4,7)'.format(vocabulary_src)
    else:
        if train_or_test=='train':
            is_train = 1
        elif train_or_test=='test':
            is_train = 0
        patents_query = 'select p.id,lower({0}) from patents p,patent_{0}_language_train_test_vw pl where p.id=pl.id and language=\'en\' and is_train={1} and {0}!=\'\' and description!=\'\' and substr(p.id,length(p.id)-1) in (\'A1\',\'A2\') group by substr(p.id,4,7)'.format(vocabulary_src,is_train)
    
    # load patents text from sqlite DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    ids = []
    corpus = []
    
    #patents_query = 'select {0} from patents group by lower(description) having description!=\'\''.format(vocabulary_src)
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(patents_query)
        while True:
            patent = cur.fetchone()
            if patent==None or patent[0]==None or patent[1]==None:
                break
            ids.append(patent[0])
            corpus.append(patent[1])
        print 'loaded {0} records.'.format(len(ids))
    return {'ids':ids,'corpus':corpus}


# In[ ]:

def load_corpus_and_labels(vocabulary_src,train_or_test):
    import sqlite3 as sqlitedb    
    from clef_globals import *

    if train_or_test not in train_or_test_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(train_or_test,train_or_test_values))
    if vocabulary_src not in vocabulary_src_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(vocabulary_src,vocabulary_src_values))
        
    if train_or_test=='both':
        patents_query = 'select p.id,lower({0}),lower(tags) from patents p,patent_{0}_language_train_test_vw pl where p.id=pl.id and language=\'en\' and {0}!=\'\' and description!=\'\' and substr(p.id,length(p.id)-1) in (\'A1\',\'A2\') group by substr(p.id,4,7)'.format(vocabulary_src)
    else:
        if train_or_test=='train':
            is_train = 1
        elif train_or_test=='test':
            is_train = 0
        patents_query = 'select p.id,lower({0}),lower(tags) from patents p,patent_{0}_language_train_test_vw pl where p.id=pl.id and language=\'en\' and is_train={1} and {0}!=\'\' and description!=\'\' and substr(p.id,length(p.id)-1) in (\'A1\',\'A2\') group by substr(p.id,4,7)'.format(vocabulary_src,is_train)
    
    # load patents text from sqlite DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    ids = []
    corpus = []
    labels = []
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(patents_query)
        while True:
            patent = cur.fetchone()
            if patent==None or patent[0]==None or patent[1]==None or patent[2]==None:
                break
            # retrieve patent text
            ids.append(patent[0])
            corpus.append(patent[1])
            # retrieve patent ipc classification codes (more that one code separated by space)
            tags = patent[2].split(' ')
            if len(tags[len(tags)-1])==0:
                labels.append(tags[0:len(tags)-1])            
            else:
                labels.append(tags)        
        print 'loaded {0} records.'.format(len(ids))
    # map labels into unique class names
    labels_dic = {}
    labels_arr = []
    for i in range(len(labels)):
        for j in range(len(labels[i])):
            if labels[i][j] not in labels_dic:
                labels_dic[labels[i][j]] = len(labels_arr)
                labels_arr.append(labels[i][j])
                labels[i][j] = len(labels_arr)-1
            else:
                labels[i][j] = labels_dic[labels[i][j]]
    return {'ids':ids,'corpus':corpus,'labels':labels,'labels_dic':labels_dic,'labels_arr':labels_arr}


# In[ ]:

def load_corpus_with_labels_mappings(vocabulary_src,train_or_test,labels_dic):
    import sqlite3 as sqlitedb    
    from clef_globals import *

    if train_or_test not in train_or_test_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(train_or_test,train_or_test_values))
    if vocabulary_src not in vocabulary_src_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(vocabulary_src,vocabulary_src_values))
        
    if train_or_test=='both':
        patents_query = 'select p.id,lower({0}),lower(tags) from patents p,patent_{0}_language_train_test_vw pl where p.id=pl.id and language=\'en\' and {0}!=\'\' and description!=\'\' and substr(p.id,length(p.id)-1) in (\'A1\',\'A2\') group by substr(p.id,4,7)'.format(vocabulary_src)
    else:
        if train_or_test=='train':
            is_train = 1
        elif train_or_test=='test':
            is_train = 0
        patents_query = 'select p.id,lower({0}),lower(tags) from patents p,patent_{0}_language_train_test_vw pl where p.id=pl.id and language=\'en\' and is_train={1} and {0}!=\'\' and description!=\'\' and substr(p.id,length(p.id)-1) in (\'A1\',\'A2\') group by substr(p.id,4,7)'.format(vocabulary_src,is_train)
    
    # load patents text from sqlite DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    ids = []
    corpus = []
    labels = []
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(patents_query)
        while True:
            patent = cur.fetchone()
            if patent==None or patent[0]==None or patent[1]==None or patent[2]==None:
                break
            # retrieve patent text
            ids.append(patent[0])
            corpus.append(patent[1])
            # retrieve patent ipc classification codes (more that one code separated by space)
            tags = patent[2].split(' ')
            if len(tags[len(tags)-1])==0:
                labels.append(tags[0:len(tags)-1])            
            else:
                labels.append(tags)        
        print 'loaded {0} records.'.format(len(ids))
    
    # map labels into unique class names
    for i in range(len(labels)):
        for j in range(len(labels[i])):
            labels[i][j] = labels_dic[labels[i][j]]
    return {'ids':ids,'corpus':corpus,'labels':labels}

