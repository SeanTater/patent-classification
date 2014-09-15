
# coding: utf-8

## CLEF-IP2010 Chi Square Vocabulary Builder

####### Build stemmed and lemmatized vocabulary (unigrams + bigrams) from clef-ip2010 corpus and selec top top n chi square features then store into DB

# In[ ]:

def build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf):    
    from clef_globals import *
    from sklearn.feature_extraction.text import CountVectorizer
    from sklearn.feature_selection import chi2    
    from sklearn.feature_selection import SelectKBest
    import heapq
    import numpy
    
    # tokenize text using initial vocabulary (achieving min_df and min_tf)
    vectorizer = CountVectorizer(tokenizer=tokenizer,ngram_range=(1,max_ngram_size),stop_words=stop_words,vocabulary=initial_vocabulary)
    corpus_vectors = vectorizer.fit_transform(corpus)

    vocabulary = dict();
    
    # apply maximum chi square threshold for each class
    print 'applying chi2 threshold on {0} labels'.format(labels.shape[0])
    for i in range(labels.shape[0]): # each row i in labels 2d-array is [0-1] class membership for class i
        # select top n features for this class label
        ch2 = SelectKBest(chi2, k=max_chi_square_terms)
        ch2.fit(corpus_vectors, labels[i])
        top_n_terms = ch2.get_support()
        #print 'found {0} top terms'.format(sum((top_n_terms&1)))
        #chi_scores,_ = chi2(corpus_vectors, labels[i])
        #nans = sum(numpy.isnan(chi_scores))
        #if nans>0:
        #    print 'chi2 returned {} nans'.format(nans)
        
        #top_n_scores = heapq.nlargest(max_chi_square_terms,chi_scores)
        #top_n_terms = numpy.array((chi_scores>=min(top_n_scores)) & 1)
        #print 'found {0} top terms with minimum chi2={1}'.format(sum(top_n_terms),min(top_n_scores))
        
        # loop on initial vocabulary and choose only top ones
        for k,v in vectorizer.vocabulary_.iteritems():
            if top_n_terms[v]==True:
                if vocabulary.has_key(k)==False: # this is a term whose chi2 is at least min chi2 and not already in vocabulary
                    vocabulary[k] = len(vocabulary)
    print 'found {0} terms'.format(len(vocabulary))
    return vocabulary


# In[ ]:

def save_vocabulary(vocabulary,tbl_name):
    # save vocabulary in DB for future use
    import sqlite3 as sqlitedb
    from clef_globals import *

    l = []
    l.extend([i] for i in vocabulary)
    con = sqlitedb.connect(db_path)
    with con:
        con.execute('drop table if exists {0}'.format(tbl_name))
        con.execute('create table {0}(term text)'.format(tbl_name))
        con.executemany('insert into {0}(term) values(?)'.format(tbl_name),l)


# In[ ]:

# build lemmatized top n chi square raw unigrams vocabulary
def build_raw_lemmatized_chi_unigrams_vocabulary(corpus,labels,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from lemmatizing_tokenizer import RawLemmaTokenizer
    
    tokenizer = RawLemmaTokenizer()
    stop_words = {}
    max_ngram_size = 1
    
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_raw_lemmas_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_raw_lemmas_chi_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

# build lemmatized top n chi square unigrams vocabulary
def build_lemmatized_chi_unigrams_vocabulary(corpus,labels,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from lemmatizing_tokenizer import LemmaTokenizer
    
    tokenizer = LemmaTokenizer()
    stop_words = {}
    max_ngram_size = 1
    
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_lemmas_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_lemmas_chi_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

# build lemmatized unigrams stopwords vocabulary
def build_lemmatized_unigrams_stopwords_vocabulary(corpus,labels,stop_words,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from lemmatizing_tokenizer import LemmaTokenizer
    
    tokenizer = LemmaTokenizer()
    max_ngram_size = 1
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_lemmas_unigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_lemmas_chi_unigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

# build lemmatized unigrams vocabulary
def build_raw_lemmatized_chi_bigrams_vocabulary(corpus,labels,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from lemmatizing_tokenizer import RawLemmaTokenizer
    from sklearn.feature_extraction.text import CountVectorizer
    
    tokenizer = RawLemmaTokenizer()
    stop_words = {}
    max_ngram_size = 2
        
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_raw_lemmas_bigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)

    # save to DB
    tbl_name = 'clef_2010_{0}_raw_lemmas_chi_bigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name    


# In[ ]:

# build lemmatized unigrams vocabulary
def build_lemmatized_bigrams_vocabulary(corpus,labels,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from lemmatizing_tokenizer import LemmaTokenizer
    
    tokenizer = LemmaTokenizer()
    stop_words = {}
    max_ngram_size = 2
        
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_lemmas_bigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)

    # save to DB
    tbl_name = 'clef_2010_{0}_lemmas_chi_bigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name    


# In[ ]:

# build lemmatized unigrams stopwords vocabulary
def build_lemmatized_bigrams_stopwords_vocabulary(corpus,labels,stop_words,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from lemmatizing_tokenizer import LemmaTokenizer
    
    tokenizer = LemmaTokenizer()
    max_ngram_size = 2
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_lemmas_bigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_lemmas_chi_bigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name    


# In[ ]:

# build stemmed unigrams vocabulary
def build_stemmed_unigrams_vocabulary(corpus,labels,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from stemming_tokenizer import StemmingTokenizer
    
    tokenizer = StemmingTokenizer()    
    stop_words = {}
    max_ngram_size = 1
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_stems_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_stems_chi_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

# build stemmed unigrams stopwords vocabulary
def build_stemmed_unigrams_stopwords_vocabulary(corpus,labels,stop_words,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from stemming_tokenizer import StemmingTokenizer
    
    tokenizer = StemmingTokenizer()    
    max_ngram_size = 1
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_stems_unigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_stems_chi_unigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

# build stemmed bigrams vocabulary
def build_stemmed_bigrams_vocabulary(corpus,labels,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from stemming_tokenizer import StemmingTokenizer
    
    tokenizer = StemmingTokenizer() 
    stop_words = {}
    max_ngram_size = 2    
    
    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_stems_bigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)
    
    # save to DB
    tbl_name = 'clef_2010_{0}_stems_chi_bigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

# build stemmed bigrams stopwords vocabulary
def build_stemmed_bigrams_stopwords_vocabulary(corpus,labels,stop_words,vocabulary_src):
    from clef_globals import *
    from clef_vocabulary_loader import load_vocabulary
    from stemming_tokenizer import StemmingTokenizer
    
    tokenizer = StemmingTokenizer() 
    max_ngram_size = 2    

    # load initial vocabulary
    initial_vocabulary_tbl_name = 'clef_2010_{0}_stems_unigrams_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    initial_vocabulary = load_vocabulary(initial_vocabulary_tbl_name)
    
    vocabulary = build_chi_vocabulary(corpus,labels,initial_vocabulary,tokenizer,stop_words,max_ngram_size,min_df,min_tf)

    # save to DB
    tbl_name = 'clef_2010_{0}_stems_chi_bigrams_stopwords_df{1}_tf{2}'.format(vocabulary_src,min_df,min_tf)
    save_vocabulary(vocabulary,tbl_name)
    print 'done '+tbl_name


# In[ ]:

def build(vocabulary_src):
    from clef_corpus_loader import load_corpus_and_labels
    from stopwords_loader import load_inquiry_stopwords
    from sklearn.preprocessing import MultiLabelBinarizer
    
    # load clef patents with class lables from DB using only vocabulary_src as main field for vocabulary (e.g., abstract, description, claims...)
    corpus_data = load_corpus_and_labels(vocabulary_src,'train')
    corpus = corpus_data['corpus']
    labels = corpus_data['labels']
    labels_dic = corpus_data['labels_dic']
    labels_arr = corpus_data['labels_arr']
    print 'done loading {0} records and {1} labels.'.format(len(corpus),len(labels_dic))
    
    # binarize the labels
    binarizer = MultiLabelBinarizer()
    binarized_labels = binarizer.fit_transform(labels)
    
    # build vocabulary without stopwords removal
    #build_lemmatized_chi_unigrams_vocabulary(corpus,binarized_labels.transpose(),vocabulary_src)
    build_raw_lemmatized_chi_unigrams_vocabulary(corpus,binarized_labels.transpose(),vocabulary_src)
    build_raw_lemmatized_chi_bigrams_vocabulary(corpus,binarized_labels.transpose(),vocabulary_src)
    #build_lemmatized_bigrams_vocabulary(corpus,binarized_labels.transpose(),vocabulary_src)
    #build_stemmed_unigrams_vocabulary(corpus,binarized_labels.transpose(),vocabulary_src)
    #build_stemmed_bigrams_vocabulary(corpus,binarized_labels.transpose(),vocabulary_src)

    # load inquiry stopwords list
    #stop_words = load_inquiry_stopwords()

    # build vocabulary with stopwords removal
    #build_lemmatized_unigrams_stopwords_vocabulary(corpus,binarized_labels.transpose(),stop_words,vocabulary_src)
    #build_lemmatized_bigrams_stopwords_vocabulary(corpus,binarized_labels.transpose(),stop_words,vocabulary_src)
    #build_stemmed_unigrams_stopwords_vocabulary(corpus,binarized_labels.transpose(),stop_words,vocabulary_src)
    #build_stemmed_bigrams_stopwords_vocabulary(corpus,binarized_labels.transpose(),stop_words,vocabulary_src)


# In[ ]:

def main():
    # build clef abstracts vocabulary
    vocabulary_src = 'abstract'
    build(vocabulary_src)

    # build clef claims vocabulary
    vocabulary_src = 'claims'
    #build(vocabulary_src)

    # build clef description vocabulary
    vocabulary_src = 'description'
    #build(vocabulary_src)

    print 'done!'


# In[ ]:

main()


# In[ ]:



