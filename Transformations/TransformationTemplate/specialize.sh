#!/bin/bash

mkdir ../$1 
sed s/TransformationTemplate/$1/g TransformationTemplate.cpp		> ../$1/$1.cpp
sed s/TransformationTemplate/$1/g TransformationTemplate.h		> ../$1/$1.h
sed s/TransformationTemplate/$1/g TransformationTemplateActions.cpp	> ../$1/$1Actions.cpp
sed s/TransformationTemplate/$1/g TransformationTemplateActions.h	> ../$1/$1Actions.h
sed s/TransformationTemplate/$1/g TransformationTemplateMatchers.cpp	> ../$1/$1Matchers.cpp
sed s/TransformationTemplate/$1/g TransformationTemplateMatchers.h	> ../$1/$1Matchers.h
