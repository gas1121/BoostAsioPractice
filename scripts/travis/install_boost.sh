#!/bin/bash

BOOST_BASENAME=boost_1_61_0
BOOST_LIBRARIES=(libboost_chrono.so libboost_date_time.so libboost_context.so libboost_regex.so libboost_system.so libboost_thread.so libboost_coroutine.so)

function isLibraryMissing {
  for LIBRARY in $BOOST_LIBRARIES; do
    if [[ ! -e ~/$BOOST_BASENAME/stage/lib/$LIBRARY ]]; then
      return 0
    fi
  done

  return 1
}

if isLibraryMissing; then
  cd ~

  wget https://downloads.sourceforge.net/project/boost/boost/1.61.0/${BOOST_BASENAME}.tar.bz2
  rm -rf $BOOST_BASENAME
  tar xf ${BOOST_BASENAME}.tar.bz2

  cd $BOOST_BASENAME
  ./bootstrap.sh
  ./b2 toolset=gcc variant=debug,release cxxflags="-std=c++11" --with-date_time --with-thread --with-regex --with-system --with-coroutine --with-context
fi