<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="Item">
    <html>
      <xsl:if test="@Language">
        <xsl:attribute name="xml:lang">
          <xsl:value-of select="@Language"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="count(@Language) = 0">
        <xsl:attribute name="xml:lang">en</xsl:attribute>
      </xsl:if>
      <head>
        <xsl:variable name="title">
          <xsl:value-of select="/Item/ItemTitle"/>
        </xsl:variable>
        <xsl:variable name="uid">
          <xsl:value-of select="/Item/FrontMatter/ISBN"/>
        </xsl:variable>
        <xsl:variable name="creator">
          <xsl:value-of select="/Item/Unit/ByLine"/>
        </xsl:variable>
        <title>
          <xsl:value-of select="/Item/ItemTitle"/>
        </title>
        <meta name="dtb:uid" content="UID-{$uid}"/>
        <meta name="dtb:title" content="{$title}"/>
        <meta name="dc:Title" content="{$title}"/>
        <meta name="dc:Creator" content="{$creator}"/>
      </head>
      <body>
        <h1>
          <xsl:value-of select="/Item/ItemTitle"/>
        </h1>
        <xsl:apply-templates select="/Item/FrontMatter"/>
        <xsl:for-each select="/Item/Unit">
          <xsl:call-template name="apply-templates-lang"/>
        </xsl:for-each>
        <xsl:apply-templates select="/Item/BackMatter"/>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
