
# coding: utf-8

## Stopwords loader

####### Load sotpwords from DB table

# In[ ]:

# load inquiry stopwords list from DB
def load_inquiry_stopwords():
    import sqlite3 as sqlitedb
    from clef_globals import *

    stop_words = set()
    stopwords_query = 'select stopword from inquiry_stopwords'
    con = sqlitedb.connect(db_path)
    with con:
        cur = con.execute(stopwords_query)
        while True:
            stopword = cur.fetchone()
            if stopword==None or stopword[0]==None:
                break
            stop_words.add(stopword[0])
    
    return stop_words

