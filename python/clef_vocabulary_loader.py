
# coding: utf-8

## CLEF-IP2010 Vocabulary Loader

####### Load clef-ip2010 patents vocabulary from DB table

# In[741]:

def load_vocabulary(tbl_name):
    import sqlite3 as sqlitedb
    from clef_globals import *

    # load vocabulary from sqlite DB
    vocabulary = []
    stmt = 'select term from {0}'.format(tbl_name)
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(stmt)
        while True:
            term = cur.fetchone()
            if term==None or term[0]==None:
                break
            # retrieve patent text
            vocabulary.append(term[0])

    print 'loaded ({0}) terms'.format(len(vocabulary))
    return vocabulary


# In[743]:

# load all unigrams from tbl_name_full1 and only bigrams existing in both tbl_name_full2&tbl_name_intersect
def load_common_vocabulary(tbl_name_full1,tbl_name_full2,tbl_name_intersect,stem_or_lemma):
    import sqlite3 as sqlitedb
    from clef_globals import *

    # load vocabulary from sqlite DB
    vocabulary = []
    #stmt = 'select term from {0} where instr(term,\' \')=0 union select {1} from {2},{3} where {4}=term'.format(tbl_name_full,stem_or_lemma,tbl_name_full,tbl_name_intersect,stem_or_lemma)
    #stmt = 'select term from {0} where term not like \'% %\' union select {1} from {2},{3} where {4}=term union select bigram from {5},{6} where bigram=term'.format(tbl_name_full,stem_or_lemma,tbl_name_full,tbl_name_intersect,stem_or_lemma,tbl_name_full,tbl_name_intersect)
    #stmt = 'select term from {0} union select {1} from {2} union select bigram from {2}'.format(tbl_name_full,stem_or_lemma,tbl_name_intersect)
    stmt = 'select term from {0} union select {1} from {2},{3} where {1}=term union select bigram from {2},{3} where bigram=term'.format(tbl_name_full1,stem_or_lemma,tbl_name_full2,tbl_name_intersect)
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(stmt)
        while True:
            term = cur.fetchone()
            if term==None or term[0]==None:
                break
            # retrieve patent text
            vocabulary.append(term[0])

    print 'loaded ({0}) terms'.format(len(vocabulary))
    return vocabulary

