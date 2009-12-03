<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template name="apply-templates-lang">
    <xsl:for-each select="@xml:lang">
      <xsl:if test="string-length(.) &gt; 0">
        <xsl:copy/>
      </xsl:if>
    </xsl:for-each>
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="language">
    <xsl:if test="string-length(@val) &gt; 0">
      <span>
        <xsl:attribute name="xml:lang">
          <xsl:value-of select="@val"/>
        </xsl:attribute>
        <xsl:apply-templates/>
      </span>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
