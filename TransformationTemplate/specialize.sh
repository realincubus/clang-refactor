#!/bin/bash

mkdir $2 
sed s/TransformationTemplate/$1/g TransformationTemplate.cpp		> $2/$1.cpp
sed s/TransformationTemplate/$1/g TransformationTemplate.h		> $2/$1.h
sed s/TransformationTemplate/$1/g TransformationTemplateActions.cpp	> $2/$1Actions.cpp
sed s/TransformationTemplate/$1/g TransformationTemplateActions.h	> $2/$1Actions.h
sed s/TransformationTemplate/$1/g TransformationTemplateMatchers.cpp	> $2/$1Matchers.cpp
sed s/TransformationTemplate/$1/g TransformationTemplateMatchers.h	> $2/$1Matchers.h
