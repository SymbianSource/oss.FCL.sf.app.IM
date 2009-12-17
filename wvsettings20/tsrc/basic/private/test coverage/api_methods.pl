#
# Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  Parse headers files and search for public methods
#

#!perl -w

use Getopt::Long;

my $help = 0;
my $output_path = ".";
GetOptions ("path=s" => \@api_path,
			"output=s" => \$output_path,
			"verbose"=> \$verbose,
			"help" => \$help);

if ($help or !@api_path){
	print "\nPath parameter is mandatory!\n" unless $help;
	print "\n";
	print "This script will parse header files looking for exported methods\n";
	print "Parameters:\n";
	print "\t --path -p		-> (mandatory) path where parsing will start, multi-parameters are allowed\n"; 
	print "\t --output -o		-> path where output will be stored\n";
	print "\t --verbose -v		-> verbose output messages\n";
	print "\t --help -h		-> print  this help\n";
	exit;	
}

my $file_location;
my $file_name;

$output_path .="\\";
`md $output_path 2>&1`;
 
foreach $api_path (@api_path){
	
	@dirs = `dir $api_path\\*.h /s/b 2>&1`;
	chomp @dirs;
	$tmp_path = $api_path;
	$tmp_path =~ s/\\/\./g;
	
	foreach (@dirs) {
		$file_location = $1 if /$tmp_path(.+)/;
		$file_name = "$1.txt" if /(\w+\.\w+)$/;
		print "Public methods from file ($file_name): $api_path$file_location\n" if $verbose;;
		open (APIFILE, "$api_path$file_location") or warn "Can not open file $file_location";
		open (OUT, ">>$output_path$file_name") or warn "Can not create file $file_name";
		
		while (<APIFILE>)
		{
			#remove white chars 
			chomp;
			s/^\s+//;
			
			#find class name
			$class = $1 if /^\s*class (\w+)/i;
			
			#concatenate lines if definision is in multiline
			if ($line){
				$line .= $_;
				next unless /;/; #next if line does not contain end of declaration
			}
			
			#search for all IMPORT_C lines
			if (/IMPORT_C/){
				$line = "$class"."::"."$1" if /IMPORT_C.+?([~\[\]\w]+\s*\(.*)/i;
				next unless /;/; #next if line does not contain full declaration
			}
		
			$line .= "\n" if /;/;
			#remove some unnesessary chars
			if ($line) {
				$line = "$1\n" if ($line =~ /(.+\))/);
				$line =~ s/^\s+// if $line;
				print "\t$line" if $verbose and $line;
				print OUT "$line" if $line;
			}
			$line = "";
		}
		close OUT;
		close APIFILE;
	}
	
}