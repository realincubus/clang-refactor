//===-- ExtractMethod/ExtractMethodActions.cpp - Matcher callback ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the definition of the ExtractMethodFixer class which is
/// used as an ASTMatcher callback. Also within this file is a helper AST
/// visitor class used to identify sequences of explicit casts.
///
//===----------------------------------------------------------------------===//

#include "ExtractMethodActions.h"
#include "ExtractMethodMatchers.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/CharInfo.h"
#include "clang/Lex/Lexer.h"

#include "Core/Utility.h"

#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace clang;

using namespace TransformationUtility;

namespace {
void ReplaceWith(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, SourceLocation EndLoc, const clang::ASTContext& Context, std::string replacement ){ 
    using namespace std;

  CharSourceRange Range(SourceRange(StartLoc, EndLoc), true);

  if ( isReplaceableRange( StartLoc, EndLoc, SM, Owner ) ){ 
      Owner.addReplacementForCurrentTU( tooling::Replacement(SM, Range, replacement ));
  }
}
void Insert(Transform &Owner, SourceManager &SM,
                        SourceLocation StartLoc, const clang::ASTContext& Context, std::string replacement ){ 
    using namespace std;

    Owner.addReplacementForCurrentTU( tooling::Replacement(SM, StartLoc, 0, replacement ));
}
}

ExtractMethodFixer::ExtractMethodFixer(unsigned &AcceptedChanges,
                           Transform &Owner)
    : AcceptedChanges(AcceptedChanges), Owner(Owner) {
}


void ExtractMethodFixer::run(const ast_matchers::MatchFinder::MatchResult &Result) {
  using namespace std;
  ASTContext& context = *Result.Context;
  SourceManager& SM = context.getSourceManager();

  {
      const DeclRefExpr* node = Result.Nodes.getNodeAs<DeclRefExpr>(MatcherExtractMethodID);

      // finds all variables which need to be extracted
      if ( node ) {
	  if ( !isReplaceableRange( node->getLocStart(),node->getLocEnd(),SM, Owner) ) return;
	  cout << "found stmt" << endl;
	  if ( Owner.isInRange( node , SM ) ) {
	     // kill doublicates
	     if ( std::find(OutsideDeclares.begin(), OutsideDeclares.end(), node->getDecl()) == OutsideDeclares.end() ) {
		 // is realy outside ?
		 if ( !Owner.isInRange( node->getDecl(), SM ) ){
		     // fill type and name
		     std::string parameter_type = node->getDecl()->getType().getAsString() + string("&");
		     std::string parameter_name = getString(node,SM);
		     std::string parameter_text = parameter_type + string(" ") + parameter_name;
		     ExtractedParameters.push_back( parameter_text );
		     ExtractedArguments.push_back( parameter_name );
		     OutsideDeclares.push_back( node->getDecl() );
		 }
	     }
	  }else{
	     // TODO hadle case that somethings refereces a declare that will be extracted
	     // possible solution :
	     //	remove text for declare from method extract and place it before function call
	     //	possible problem :
	     //	
	     //	int a = 0;
	     //	int b = a;
	     //
	     //	b = b + 1;
	     //
	     //	b will then be initialized by a non defined variable a
	     //
	     if ( Owner.isInRange( node->getDecl(), SM ) ){
		 exit(-1);
	     }
	  }
      }
  }

  {
    const FunctionDecl* node = Result.Nodes.getNodeAs<FunctionDecl>(MatcherExtractMethodCompoundID);

    if ( node ) {
	// TODO check that the node text is inside of this function decl 
	auto isInRange = [&]( SourceRange OutRange, SourceRange InRange ){
	    auto out_line_begin = SM.getSpellingLineNumber( OutRange.getBegin() );
	    auto out_column_begin = SM.getSpellingColumnNumber( OutRange.getBegin() );
	    auto out_line_end = SM.getSpellingLineNumber( OutRange.getEnd() );
	    auto out_column_end = SM.getSpellingColumnNumber( OutRange.getEnd() );

	    auto in_line_begin = SM.getSpellingLineNumber( InRange.getBegin() );
	    auto in_column_begin = SM.getSpellingColumnNumber( InRange.getBegin() );
	    auto in_line_end = SM.getSpellingLineNumber( InRange.getEnd() );
	    auto in_column_end = SM.getSpellingColumnNumber( InRange.getEnd() );

	    // test begin
	    auto test_begin = [&](){
		if ( in_line_begin > out_line_begin ){
		    return true;
		}else if ( in_line_begin == out_line_begin ) {
		    if ( in_column_begin >= out_column_begin ) {
			return true;
		    }
		}
		return false;
	    };
	    auto test_end = [&](){
		if ( in_line_end < out_line_end ){
		    return true;
		}else if ( in_line_end == out_line_end ) {
		    if ( in_column_end >= out_column_end ) {
			return true;
		    }
		}
		return false;	
	    };
	    if ( test_begin() && test_end() ) return true;
	    return false;

	};

	if ( !isReplaceableRange( node->getLocStart(),node->getLocEnd(),SM, Owner) ) return;
	// TODO this is misplaced. shoud be done once
	// save the actual sourcetext 
	assert( !Owner.line_ranges.empty() && "no source selection given" );
	auto begin_loc = SM.translateLineCol( SM.getFileID(node->getLocStart()), Owner.line_ranges.front().line_begin, Owner.line_ranges.front().column_begin );
	auto end_loc = SM.translateLineCol( SM.getFileID(node->getLocStart()), Owner.line_ranges.front().line_end, Owner.line_ranges.front().column_end );
	SourceRange sr(begin_loc,end_loc);
	if ( !isInRange( node->getSourceRange(), sr ) ) return;

	ExtractedStatements = getString(sr,SM) ;
	std::string function_call_text = "function( ";
	for( auto it = ExtractedArguments.begin(), end = ExtractedArguments.end() ; it != end ; it++ ){
	    if ( distance( it , end ) == 1 ){
		function_call_text += *it ;
	    }else{
		function_call_text += *it + string(", ");
	    }
	}
	function_call_text += " );";
	ReplaceWith( Owner, SM, sr.getBegin(), sr.getEnd(), context, function_call_text ); 

	// generate the function text here and place it infront of the function we are in	
	std::string extracted_function_text = "void function( ";
	for( auto it = ExtractedParameters.begin(), end = ExtractedParameters.end() ; it != end ; it++ ){
	    if ( distance(it,end) == 1 ) {
		extracted_function_text += *it;
	    }else{
		extracted_function_text += *it + string(", ");
	    }
	}
	extracted_function_text += " ){\n";
	extracted_function_text += ExtractedStatements + string("\n") ;
	extracted_function_text += "}\n\n";
	Insert( Owner, SM, node->getLocStart(), context, extracted_function_text );

    }

  }
      // TODO replace all but one extracted statement with no text

}














