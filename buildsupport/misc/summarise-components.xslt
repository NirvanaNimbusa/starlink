<?xml version="1.0"?>

<!--
  This stylesheet constructs a summary of the information in a
  componentset instance, in particular the componentset.xml at the top
  of the build tree.

  It produces a table line for each component listed, giving the
  component name, its path in the repository (linked to the cvsweb
  server), and the person who is listed as the component's `owner'.
  If there is no `owner', then it prints out the names of all the
  listed developers.
-->

<!DOCTYPE x:stylesheet SYSTEM "xslt.dtd">

<x:stylesheet version="1.0"
  xmlns:x="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <x:variable name="cvsweb"
    >http://cvsweb.starlink.ac.uk/cvsweb.cgi</x:variable>

  <x:output method="html"/>

  <x:template match="componentset">

    <x:text>

    </x:text>
    <x:comment>This list of component owners is generated from the
      information in componentset.xml, in the CVS top-level, which
      is in turn generated from the component.xml files in each
      component directory.  DO NOT EDIT THIS FILE.</x:comment>
    <x:text>

    </x:text>

    <html>
      <head>
        <title>Component summary</title>
        <style type="text/css">
body {
  color: #003;
  background: white;
  margin-left: 10%;
  margin-right: 10%;
  font-family: optima, arial, helvetical, sans-serif };
  font-size: smaller;
}

/* Link colours should be specified, since text colour was in body spec */
a:link    { color: #009; }
a:active: { color: #00C; }
a:visited { color: #600; }
a[href]:hover { background: #FF9; }     /* CSS2 */

th {
  text-align: left;
}
tr.r0 {
  background: #DDF;
}
tr.r1 {
  background: #BBF;
}
        </style>
      </head>
      <body>
        <h1>Component summary</h1>

        <p>See also the <a href="http://cvsweb.starlink.ac.uk"
            >CVS repository</a>.</p>

        <table>
          <tr>
            <th>Component</th>
            <th>Repository path</th>
            <th>Contact</th>
          </tr>
          <x:apply-templates select="component">
            <x:sort select="@id"/>
          </x:apply-templates>
        </table>
      </body>
    </html>
  </x:template>

  <x:template match="component">
    <tr class="r{position() mod 2}">
      <td>
        <x:value-of select="@id"/>
      </td>
        <td>
        <a href="{$cvsweb}/{path}/">
          <x:apply-templates select="path"/>
        </a>
      </td>
      <td>
        <x:apply-templates select="developers"/>
      </td>
    </tr>
    <x:text>
</x:text>
  </x:template>

  <x:template match="developers">
    <x:choose>
      <x:when test="person[role='owner']">
        <x:apply-templates select="person[role='owner']" mode='with-email'/>
      </x:when>
      <x:otherwise>
        <x:apply-templates select="person"/>
      </x:otherwise>
    </x:choose>
  </x:template>

  <x:template match="person">
    <x:if test="position() &gt; 1">
      <x:text>, </x:text>
    </x:if>
    <x:value-of select="name"/>
  </x:template>

  <x:template match="person" mode="with-email">
    <x:if test="position() &gt; 1">
      <x:text>, </x:text>
    </x:if>
    <x:choose>
      <x:when test="email">
        <a href="mailto:{email}">
          <x:value-of select="name"/>
        </a>
      </x:when>
      <x:otherwise>
        <x:value-of select="name"/>
      </x:otherwise>
    </x:choose>
  </x:template>

</x:stylesheet>

