#!/usr/local/bin/perl -w

#+
#  Name:
#     scb.pl

#  Purpose:
#     Generate listing of a Starlink source code file.

#  Language:
#     Perl 5

#  Description:
#     This script extracts the source code for a routine in the USSC.  
#     The only input value it takes is the name of the routine, which
#     must be the key of an entry in the index dbm file generated by
#     scbindex.
#
#     It operates in two modes:
#        text:  prints the source file raw.
#        HTML:  prints the source code with HTML markup.
#
#     Normally, it will choose between the two modes according to whether 
#     it appears to have been called as a CGI program or not.  This can
#     be overridden to produce HTML output from the command line however.

#  Authors:
#     MBT: Mark Taylor (IoA, Starlink)
#     {enter_new_authors_here}

#  History:
#     25-AUG-1998 (MBT):
#       Initial revision.
#     {enter_further_changes_here}

#  Bugs:
#     {note_any_bugs_here}

#-

#  File locations.

$indexfile = "/local/devel/scb/index";
$taskfile  = "/local/devel/scb/tasks";

#  Directory locations.

$tmpbase = "/local/junk/scb";    # scratch directory
$tmpdir = "$tmpbase/$$";

#  Name of this program relative to this program.

$self = $0;
$self =~ s%.*/%%;
$scb = $self;
$usage = "Usage: $self <module>\n";

#  Required libraries.

use Fcntl;
use SDBM_File;
use libscb;

#  Declarations.

sub get_module;
sub query_form;
sub extract_file;
sub output;
sub error;
sub header;
sub footer;
sub hprint;
sub tidyfiles;

#  Determine operating environment.

$cgi = defined $ENV{'SERVER_PROTOCOL'};
print "Content-Type: text/html\n\n" if ($cgi);
$html = $cgi;

#  Parse arguments.

#  Get argument list from command line or CGI environment variable.

@args = $cgi ? split ('&', $ENV{'QUERY_STRING'})
             : @ARGV;

#  Extract named arguments (of format arg=value).

if (@args) {
   for ($i = $#args; $i>=0; $i--) {
      if ($args[$i] =~ /(.*)=(.*)/) {
         $arg{$1} = $2;
         splice (@args, $i, 1);
      }
   }
}

#  If chosen variables still have no value pick them up by order on 
#  command line.

$arg{'module'}  ||= shift @args;
$arg{'package'} ||= shift @args;
$arg{'package'} ||= '';

#  Open index file, tied to index hash %locate.

   tie %locate, SDBM_File, $indexfile, O_RDONLY, 0644;

#  Main processing: either pull out requested module, or present a form.

if ($arg{'module'}) {
   get_module uc ($arg{'module'}), uc ($arg{'package'});
}
else {
   if ($cgi) {
      query_form uc ($arg{'package'});
   }
   else {
      $0 =~ s%.*/%%;
      die $usage;
   }
}

#  End

exit;

########################################################################
# Subroutines.
########################################################################


########################################################################
sub query_form {

#  CGI output of the program when no arguments have been specified.

#  Arguments.

   $package = shift;

#  Read file listing packages and (probably) tasks.

   my $pack;
   open TASKS, $taskfile or error "Couldn't open $taskfile";
   while (<TASKS>) {
      ($pack, @tasks) = split;
      $pack =~ s/:?$//;
      @{$tasks{$pack}} = @tasks;
   }
   close TASKS;
   @packages = sort keys %tasks;

#  Print form header.

   header $self;
   hprint "
      <h1>$self: Starlink Source Code Browser</h1>
      <form method=GET action='$self'>
   ";

#  Print query box for module.

   hprint "
      Name of source module:
      <input name=module size=24 value=''>
      <br>
   ";

#  Print query box for package.

   my $selected = $package ? '' : 'selected';
   hprint "
      Name of package (optional):
      <font size=-1>
      <select name=package>
      <option value='' $selected> Any
   ";
   for $pack (@packages) {
      $selected = $pack eq $package ? 'selected' : '';
      print "<option value=$pack $selected>$pack\n";
   }
   print "</select></font>\n";

#  Print submission button and form footer.

   hprint "
      <br>
      <input type=submit value='Retrieve'>
      </form>
      <hr>
   ";

   if ($package) {

#     Give some indication of contents of package.

      print "<h2>$package</h2>\n";


      my @tasks = @{$tasks{$package}};

      if (@tasks) {

#        Print list of (maybe) tasks for selected package.

         hprint "
            <h3>Tasks</h3>
            The following appear to be tasks within package $package:
            <br>
         ";
         foreach $task (sort @tasks) {
            print "<a href='$scb?$task&$package#$task'>$task</a>\n";
         }
         print "<hr>\n";
      }

#     Go through list of modules, picking ones from the selected package
#     only, and grouping them by prefix.

      my (%modules, $mod, $loc, $tail);
      while (($mod, $locs) = each %locate) {
         foreach $loc (split ' ', $locs) {
            if ($loc =~ /^$package#/io) {
               $mod =~ /^([^_]*_)/;
               $prefix = $1 || '';
               push @{$modules{$prefix}}, $mod;
            }
         }
      }

      if (%modules) {

#        Print list of all modules in package.

         hprint "
            <h3>Modules</h3>
            The following modules (C and Fortran functions, subroutines
            and include files) from the package $package are indexed:<br>
         ";
         print "<dl>\n";
         my ($prefix, $r_mods);
         foreach $prefix (sort keys %modules) {
            print "<dt> $prefix <br>\n<dd>\n";
            foreach $mod (sort @{$modules{$prefix}}) {
               print "<a href='$scb?$mod&$package#$mod'>$mod</a>\n";
            }
         }
         print "</dl>\n<hr>\n";
      }
            
      unless (%modules || @tasks) {

#        This shouldn't really happen.

         hprint "
            Apparently there are no indexed modules for the package $package.
         "; 
      }

   }
   else {

#     Print list of all packages.

      foreach $pack (@packages) {
         print "<a href='$scb?module=&package=$pack'>$pack</a></br>\n";
      }
   }
   footer;

}
   



########################################################################
sub get_module {

#  This routine takes the name of a module, locates it using the index
#  dbm, and outputs it in an appropriate form.

#  Arguments.

   $module = shift;
   $package = shift;

#  Set up scratch directory.

   mkdir "$tmpdir", 0777;
   chdir "$tmpdir"  or error "Failed to enter $tmpdir";

#  Get logical path name from database.

   @locations = split ' ', $locate{$module};

#  Generate an error if no module of the requested name is indexed.

   unless (@locations) {
      error "Failed to find module $module",
         "Probably this is a result of a deficiency in the source
          code indexing program, but it may be because the module you
          requested doesn't exist, or because the index 
          database <code>$indexfile</code> has become out of date.";
   }

#  See if any of the listed locations is in the requested package;
#  otherwise just pick any of them (in fact, the last).

   my ($head, $tail);
   foreach $location (@locations) {
      $locname = $location;
      $location =~ /^(.+)#(.+)/i;
      ($head, $tail) = (lc ($1), $2);
      last if (uc ($head) eq uc ($package));
   }

#  Interpret the first element of the location as a package or symbolic
#  directory name.  Either way, change it for a logical path name.

   my ($file, $tarfile);
   if (-d "$srcdir/$head") {
      $file = "$srcdir/$head/$tail";
   }
   elsif ( defined ($tarfile = <$srcdir/$head.tar*>) && -f $tarfile) {
      $file = "$tarfile>$tail";
   }
   elsif ($head =~ /^INCLUDE$/i) {
      $file = "$incdir/$tail";
   }
   else {
      error "Failed to interpret location $location",
         "Probably the index file $indexfile is outdated or corrupted.";
   }

#  Extract file from logical path.

   extract_file $file, $head;

#  Finished with STDOUT; by closing it here the CGI user doesn't have to
#  wait any longer than necessary (I think).

   close STDOUT;

#  Tidy up.

   tidyfiles;

}

########################################################################
sub extract_file {

#  This routine takes as argument the logical path name of a file, 
#  and, by calling itself recursively to extract files from tar 
#  archives, finishes by calling routine 'output' with a filename
#  (possibly relative to the current directory) containing the 
#  requested module.

#  Arguments.

   my $location = shift;
   my $package = shift;

   $location =~ /^([^>]+)>?([^>]*)(>?.*)$/;
   ($file, $tarcontents, $tail) = ($1, $2, $3);
   if ($tarcontents) {
      tarxf $file, $tarcontents;
      extract_file "$tarcontents$tail", $package;
      unlink $tarcontents;
   }
   else {
      output $file, $package;
   }
}


########################################################################
sub output {

#  Arguments.

   my $file = shift;              #  Filename of file to output.
   my $package = shift;

#  Get file type.

   $file =~ m%\.([^/]*)$%;
   my $ftype = $1;
   $ftype = 'f'   if ($ftype eq 'gen');
   $ftype = 'c'   if ($ftype eq 'h');

#  Open module source file.

   open FILE, $file 
      or error "Failed to open $file - index may be outdated.";

#  Output appropriate header text.

   if ($html) {
      header $locname;
      print "<pre>\n" if ($html);
   }
   else {
      print STDERR "$locname\n";
   }

   my ($body, $name, @names, $include, $sub);
   while (<FILE>) {
      if ($html) {

#        Identify active part of line.

         if ($ftype eq 'f') {

            $body = /^[cC*]/ ? '' : $_;     #  Ignore comments.
            if ($body) {
               $body =~ s/^......//;        #  Discard first six characters.
               $body =~ s/!.*//;            #  Discard inline comments.
               $body =~ s/\s//g;            #  Discard spaces.
               $body =~ y/a-z/A-Z/;         #  Fold to upper case.
            }
         }

         elsif ($ftype eq 'c') {

            $body = $_;
            $body =~ s%/\*.*\*/%%g;         #  Discard comments fully inline.
            $body =~ s%/\*.*%%;             #  Discard started comments.
         }

#        Substitute for HTML meta-characters.

         s%>%&gt;%g;
         s%<%&lt;%g;

         if ($body) {

#           Identify and deal with lines beginning modules.

            if ($name = module_name $ftype, $_) {

#              Embolden module name.

               s%($name)%<b>$1</b>%i;

#              Add anchors (multiple ones if generic function).

               @names = ($name);
               if ($name =~ /^(.*)&LT;T&GT;(.*)/) {
                  ($g1, $g2) = ($1, $2);
                  @names = map "$g1$_$g2", qw/I R D L C B UB W UW/; 
               }
               foreach $name (@names) {
                  s/^/<a name='$name'>/;
               }

            }

            if ($ftype eq 'f') {

#              Identify and deal with fortran includes.

               if ($body =~ /\bINCLUDE['"]([^'"]+)['"]/) {
                  $include = $1;
                  s%$include%<a href='$scb?$include'>$include</a>%;
               }

#              Identify and deal with fortran subroutine calls.

               if ($body =~ /\bCALL(\w+)[^=]*$/) {
                  $sub = $1;
                  s%$sub%<a href='$scb?$sub&$package#$sub'>$sub</a>%;
               }
            }
            elsif ($ftype eq 'c') {

#              Identify and deal with C calls to fortran routines.

               if ($body =~ /F77_CALL\s*\(\s*(\w+)\s*\)/) {
                  $sub = $1;
                  $module = uc $sub;
                  s%$sub%<a href='$scb?$module&$package#$module'>$sub</a>%;
               }

#              Identify and deal with C includes.

               if ($body =~ /#include\s*"\s*(\S+)\s*"/) {
                  $include = $1;
                  s%$include%<a href='$scb?$include'>$include</a>%;
               }
            }
         }
      }

#     Output (modified or unmodified) line of source.

      print;
   }
   close FILE;

#  Output appropriate footer text.

   footer;

}


########################################################################
sub tidyfiles {

#  Delete temporary directory in which files were unpacked.
#
#  Note use of "die" here rather than "error" this routine is called by
#  error, and we don't want to get into an infinite loop.

#  Maintain a healthy respect for rm -rf.

   die "Warning: $tmpdir does not match $tmpbase" 
      unless ($tmpdir =~ /^$tmpbase/);

#  Remove the directory conaining all temporary files.

   system "rm -rf $tmpdir" and die "rm -rf $tmpdir: $?\n";
}


########################################################################
sub error {

#  Arguments.

   my ($message, $more) = @_;

   tidyfiles;

   if ($html) {
      header "Error";
      print "<h1>Error</h1>\n";
      hprint "<b>$message</b>\n";
      hprint "<p>\n$more\n" if $more;
      footer;
      print STDERR "$self: $message\n" if ($cgi);
      exit 1;
   }
   else {
      die "$message\n";
   }

}



########################################################################
sub header {

#  Arguments.

   my $title = shift;

   if ($html) {
      print "<html>\n";
      print "<head><title>$title</title></head>\n";
      print "<body>\n";
   }
}


########################################################################
sub footer {
   print "</body>\n</html>\n" if $html;
}


########################################################################
sub hprint {

#  Utility routine - this just prints a string after first stripping 
#  leading whitespace from each line.  Its only purpose is to
#  allow here-documents which don't mess up the indenting of the 
#  perl source.

   local $_ = shift;
   s%^\s*%%mg;
   print;
}
