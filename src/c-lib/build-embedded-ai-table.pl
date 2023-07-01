#!/usr/bin/perl -Tw

#
#  This script can be used to build the embedded AI table (in ai.c) from the
#  Syntax Dictionary.
#
#      cat gs1-syntax-dictionary.txt | ./build-embedded-ai-table.pl > aitable.inc
#

use strict;


use constant NUM_PARTS => 5;
use constant NUM_LINTERS => 2;


my $ai_rx = qr/
    (
        (0\d)
    |
        ([1-9]\d{1,3})
    )
/x;

my $ai_rng_rx = qr/${ai_rx}(-${ai_rx})?/;

my $flags_rx = qr/[\*]+/;

my $type_mand_rx = qr/
    [XNYZ]
    (
        ([1-9]\d?)
        |
        (\.\.[1-9]\d?)
    )
/x;

my $type_opt_rx = qr/
    \[${type_mand_rx}\]
/x;

my $type_rx = qr/
    (
        ${type_mand_rx}
        |
        ${type_opt_rx}
    )
/x;

my $comp_rx = qr/
    ${type_rx}
    (,\w+)*
/x;

my $spec_rx = qr/
    ${comp_rx}
    (\s+${comp_rx})*
/x;

my $keyval_rx = qr/
    (
        (\w+)
        |
        (\w+=\S+)
    )
/x;

my $title_rx = qr/\S.*\S/;

# 123  *  N13,csum,key X0..17  req=01,321 ex=42,101 dlpkey=22,10  # EXAMPLE TITLE
my $entry_rx = qr/
    ^
    (?<ais>${ai_rng_rx})
    (
        \s+
        (?<flags>${flags_rx})
    )?
    \s+
    (?<spec>${spec_rx})
    (
        \s+
        (?<keyvals>
            (${keyval_rx}\s+)*
            ${keyval_rx}
        )
    )?
    (
        \s+
        \#
        \s
        (?<title>${title_rx})
    )?
    \s*
    $
/x;

print "static struct aiEntry embedded_ai_table[] = {\n";

while (<>) {

    chomp;

    $_ =~ /^#/ and next;
    $_ =~ /^\s*$/ and next;

    $_ =~ $entry_rx or die "Bad entry: $_";

    my $ais = $+{ais};
    my $flags = $+{flags} || '';
    my $spec = $+{spec};
    my $keyvals = $+{keyvals} || '';
    my $title = $+{title} || '';

    my @parts = split(/\s+/, $spec, NUM_PARTS);
    $#parts = NUM_PARTS - 1;

    my $specstr = '';
    foreach (@parts) {

        if (!defined($_)) {
            $specstr .= ' __,';
            next;
        }

        (my $cset, my $linters) = split(',', $_, 2);

        (my $opt, $cset, my $len) = $cset =~ /^(\[?)(.)(.*?)\]?$/;

        $opt = $opt ? 'OPT' : 'MAN';
        $len = "1$len" if $len =~ /^\.\./;
        $len = "$len..$len" if $len !~ /\./;
        (my $min, my $max) = $len =~ /^(\d+)\.\.(\d+)$/;

        $linters = $linters || '';
        my @linters = split(',', $linters, NUM_LINTERS);
        $#linters = NUM_LINTERS - 1;
        $linters = '';
        foreach (@linters) {
            $_ = '_' unless (defined $_);
            $linters .= "$_,";
        }
        $linters =~ s/^\s+|\s+$//g;

        $specstr .= " $cset,$min,$max,$opt,$linters";

    }

    $ais = "$ais-$ais" if $ais !~ /-/;
    (my $aimin, my $aimax) = $ais =~ /^(\d+)-(\d+)$/;

    my $fnc1 = $flags =~ /\*/ ? 'NO_FNC1' : 'DO_FNC1';

    $specstr = sprintf("%-100s", $specstr);

    $keyvals =~ s/\s+/ /g;
    $keyvals = sprintf("%-53s", "\"$keyvals\",");

    $title =~ s/²/^2/;
    $title =~ s/³/^3/;
    $title = sprintf("%-27s", "\"$title\"");

    for ($aimin..$aimax) {
        $_ = sprintf('%-6s', "\"$_\"");
        print "\tAI_ENTRY( $_, $fnc1,$specstr$keyvals$title ),\n";
    }

}

print "\tAI_ENTRY_TERMINATOR\n";
print "};\n";
