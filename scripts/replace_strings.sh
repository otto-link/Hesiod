#!/bin/bash
DIRS="Hesiod/src"

search=("NEW_ATTR_BOOL(" \
	    "NEW_ATTR_INT(" \
	    "NEW_ATTR_FILENAME(" \
	    "NEW_ATTR_FLOAT(" \
	    "NEW_ATTR_MAPENUM(" \
	    "NEW_ATTR_RANGE(" \
	    "NEW_ATTR_SEED(" \
	    "NEW_ATTR_WAVENB(" \
	    #
	    "GET_ATTR_BOOL" \
	    "GET_ATTR_INT" \
	    "GET_ATTR_FLOAT" \
	    "GET_ATTR_MAPENUM" \
	    "GET_ATTR_RANGE" \
	    "GET_ATTR_SEED" \
	    "GET_ATTR_WAVENB" \
	    #
	    "computingStarted()" \
	    "computingFinished()" \
       )

replace=("NEW(BoolAttribute, " \
	     "NEW(IntAttribute, " \
	     "NEW(FilenameAttribute, " \
	     "NEW(FloatAttribute, " \
	     "NEW(EnumAttribute, " \
	     "NEW(RangeAttribute, " \
	     "NEW(SeedAttribute, " \
	     "NEW(WaveNbAttribute, " \
	     #
	     "this->get_attr<BoolAttribute>" \
	     "this->get_attr<IntAttribute>" \
	     "this->get_attr<FloatAttribute>" \
	     "this->get_attr<EnumAttribute>" \
	     "this->get_attr<RangeAttribute>" \
	     "this->get_attr<SeedAttribute>" \
	     "this->get_attr<WaveNbAttribute>" \
	     #
	     "compute_started(this->get_id())" \
	     "compute_finished(this->get_id())" \
	)

len=${#search[@]}
for ((i=0; i<$len; i++)); do
    echo "${search[$i]} -> ${replace[$i]}"
done
    
for D in ${DIRS}; do
    echo ${D}
    for ((i=0; i<$len; i++)); do
	find "${D}" -type f -name "*.cpp" -exec sed -i "s/${search[$i]}/${replace[$i]}/g" {} +
    done	
done
