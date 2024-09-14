#!/bin/bash
DIRS="Hesiod/src"

search=("NEW_ATTR_BOOL" \
	    "NEW_ATTR_MAPENUM" \
	    "NEW_ATTR_RANGE" \
	    "NEW_ATTR_SEED" \
	    "NEW_ATTR_WAVENB" \
	    #
	    "GET_ATTR_BOOL" \
	    "GET_ATTR_MAPENUM" \
	    "GET_ATTR_RANGE" \
	    "GET_ATTR_SEED" \
	    "GET_ATTR_WAVENB" \
       )

replace=("create_attr<BoolAttribute>" \
	     "create_attr<MapEnumAttribute>" \
	     "create_attr<RangeAttribute>" \
	     "create_attr<SeedAttribute>" \
	     "create_attr<WaveNbAttribute>" \
	     #
	     "this->get_attr<BoolAttribute>" \
	     "this->get_attr<MapEnumAttribute>" \
	     "this->get_attr<RangeAttribute>" \
	     "this->get_attr<SeedAttribute>" \
	     "this->get_attr<WaveNbAttribute>" \
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
