
# coding: utf-8

## REUTERS 21578 Corpus Loader

####### Load reuters 21578 news corpus from DB table

# In[ ]:

def load_corpus(target_topic,vocabulary_src,train_or_test):
    import sqlite3 as sqlitedb
    from reuters_globals import *

    if train_or_test not in train_or_test_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(train_or_test,train_or_test_values))
    if vocabulary_src not in vocabulary_src_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(vocabulary_src,vocabulary_src_values))
        
    if train_or_test=='both':
        if vocabulary_src=='all':
            query = 'select r.id,lower(title)||" "||lower(body) from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}" union select id,lower(title)||" "||lower(body) from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic)
        else:
            query = 'select r.id,lower({1}) from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}" union select id,lower({1}) from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,vocabulary_src)
    else:
        if train_or_test=='train':
            is_train = 1
        elif train_or_test=='test':
            is_train = 0
        if vocabulary_src=='all':
            query = 'select r.id,lower(title)||" "||lower(body) from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={1} and topic="{0}" union select id,lower(title)||" "||lower(body) from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={1} and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,is_train)
        else:
            query = 'select r.id,lower({1}) from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={2} and topic="{0}" union select id,lower({1}) from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={2} and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,vocabulary_src,is_train)
   
    # load reuters text from sqlite DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    ids = []
    corpus = []
    
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(query)
        while True:
            record = cur.fetchone()
            if record==None or record[0]==None or record[1]==None:
                break
            ids.append(record[0])
            corpus.append(record[1])
        print 'loaded {0} records.'.format(len(ids))
    return {'ids':ids,'corpus':corpus}


# In[ ]:

def load_corpus_and_labels(target_topic,vocabulary_src,train_or_test):
    import sqlite3 as sqlitedb
    from reuters_globals import *

    if train_or_test not in train_or_test_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(train_or_test,train_or_test_values))
    if vocabulary_src not in vocabulary_src_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(vocabulary_src,vocabulary_src_values))
        
    if train_or_test=='both':
        if vocabulary_src=='all':
            query = 'select r.id,lower(title)||" "||lower(body), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}" union select id,lower(title)||" "||lower(body), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic)
        else:
            query = 'select r.id,lower({1}), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}" union select id,lower({1}), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,vocabulary_src)
    else:
        if train_or_test=='train':
            is_train = 1
        elif train_or_test=='test':
            is_train = 0
        if vocabulary_src=='all':
            query = 'select r.id,lower(title)||" "||lower(body), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={1} and topic="{0}" union select id,lower(title)||" "||lower(body), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={1} and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,is_train)
        else:
            query = 'select r.id,lower({1}), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={2} and topic="{0}" union select id,lower({1}), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={2} and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,vocabulary_src,is_train)
   
    # load docs text from sqlite DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    ids = []
    corpus = []
    labels = []
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(query)
        while True:
            record = cur.fetchone()
            if record==None or record[0]==None or record[1]==None or record[2]==None:
                break
            # retrieve record text
            ids.append(record[0])
            corpus.append(record[1])
            # retrieve topics (more that one topic separated by space)
            record = record[2].split(',')
            if len(record[len(record)-1])==0:
                labels.append(record[0:len(record)-1])            
            else:
                labels.append(record)        
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

def load_corpus_with_labels_mappings(target_topic,vocabulary_src,train_or_test,labels_dic):
    import sqlite3 as sqlitedb
    from reuters_globals import *

    if train_or_test not in train_or_test_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(train_or_test,train_or_test_values))
    if vocabulary_src not in vocabulary_src_values:
        raise ValueError('\'{0}\' is an invalid value. use {1}'.format(vocabulary_src,vocabulary_src_values))
    
    if train_or_test=='both':
        if vocabulary_src=='all':
            query = 'select r.id,lower(title)||" "||lower(body), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}" union select id,lower(title)||" "||lower(body), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic)
        else:
            query = 'select r.id,lower({1}), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}" union select id,lower({1}), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt where r.id=rt.doc_id and t.id=rt.topic_id and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,vocabulary_src)
    else:
        if train_or_test=='train':
            is_train = 1
        elif train_or_test=='test':
            is_train = 0
        if vocabulary_src=='all':
            query = 'select r.id,lower(title)||" "||lower(body), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={1} and topic="{0}" union select id,lower(title)||" "||lower(body), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={1} and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,is_train)
        else:
            query = 'select r.id,lower({1}), "{0}" from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={2} and topic="{0}" union select id,lower({1}), "not_{0}" from reuters21578 where id not in (select r.id from reuters21578 r,reuters21578_topics t,reuters21578_topics_join rt,reuters21578_{0}_train_test tt where r.id=rt.doc_id and t.id=rt.topic_id and r.id=tt.id and is_train={2} and topic="{0}") and id in (select distinct(doc_id) from reuters21578_topics_join)'.format(target_topic,vocabulary_src,is_train)
   
    # load docs text from sqlite DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    ids = []
    corpus = []
    labels = []
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(query)
        while True:
            record = cur.fetchone()
            if record==None or record[0]==None or record[1]==None or record[2]==None:
                break
            # retrieve record text
            ids.append(record[0])
            corpus.append(record[1])
            # retrieve topics (more that one topic separated by space)
            record = record[2].split(',')
            if len(record[len(record)-1])==0:
                labels.append(record[0:len(record)-1])            
            else:
                labels.append(record)        
        print 'loaded {0} records.'.format(len(ids))
    
    # map labels into unique class names
    for i in range(len(labels)):
        for j in range(len(labels[i])):
            labels[i][j] = labels_dic[labels[i][j]]
    return {'ids':ids,'corpus':corpus,'labels':labels}

