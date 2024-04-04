#!/usr/bin/perl
print "Content-type: text/html\n\n"; # En-tête de la réponse
print "<HTML><HEAD><TITLE>PERL Search Results</TITLE></HEAD>\n";
print "<BODY bgcolor=#FFFFFF>\n";
print<<"TAG";
<!– ENTETE DU TABLEAU -->
<FONT SIZE=+1><B>CGI en Perl - les liens</B></FONT>
<HR>
<TABLE BORDER=2>
<TR><TD><B>Type</B></TD>
<TD><B>Links</B></TD>
<TD><B>Description</B></TD></TR>
TAG

# Liste des liens du fichier:
print "<CENTER>\n";
open (LINKDATA,"links.txt"); # Ouverture du fichier de données
while (<LINKDATA>) { # Tant qu'il y a des données dans le fichier
	# Si la ligne ne commence pas par # et n'est pas vide:
	if ((!(/#.*/))&&(/.+/)){ 
		@data = split (/\t/,$_); # On découpe la ligne par tabulation
		$type = $data[0]; # Le type est dans la première colonne
		if ($data[0] eq $type_before) { # Si le type n'a pas changé
			$data[0]=""; # Le type ne sera pas affiché
		}
		$type_before = $type; # Le type d'avant
		print "<TR><TD>$data[0]</TD>"; # Affichage du type
		# Affichage du lien:
	 	print "<TD><A HREF=\"$data[1]\" TARGET=\"_blank\">";
		print "$data[1]</A></TD>";
		# Affichage de la description:
		print "<TD>$data[2]</TD></TR>\n"; 
	}
}
print "</TABLE>"; # Fin du tableau
close (LINKDATA); # Fermeture du fichier
print <<"TAG"; # Dernier affichage
<HR><DIV ALIGN="center"><FONT SIZE=-2>Page réalisé par
<A>HREF="mailto:christian\@favier.as">Christian FAVIER</A></FONT></DIV>
</CENTER></BODY></HTML>
TAG
