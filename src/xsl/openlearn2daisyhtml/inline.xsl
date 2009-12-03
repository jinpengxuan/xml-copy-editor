<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="b">
    <!-- b not available -->
    <strong>
      <xsl:apply-templates/>
    </strong>
  </xsl:template>
  <xsl:template match="i">
    <!-- i not available -->
    <em>
      <xsl:apply-templates/>
    </em>
  </xsl:template>
  <xsl:template match="br">
    <br/>
  </xsl:template>
  <xsl:template match="u">
    <!-- deprecated -->
    <em>
      <xsl:apply-templates/>
    </em>
  </xsl:template>
  <xsl:template match="sup">
    <sup>
      <xsl:apply-templates/>
    </sup>
  </xsl:template>
  <xsl:template match="sub">
    <sub>
      <xsl:apply-templates/>
    </sub>
  </xsl:template>
  <xsl:template match="smallCaps">
    <xsl:variable name="small" select="'abcdefghijklmnopqrstuvwxyz'"/>
    <xsl:variable name="caps" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>
    <xsl:value-of select="translate(text(), $small, $caps)"/>
  </xsl:template>
  <xsl:template match="SideNote">
    <br/>
    <hr/>
    <span class="required-prodnote">Side note</span>
    <br/>
    <xsl:apply-templates/>
    <span class="optional-prodnote">End of side note</span>
    <hr/>
    <br/>
  </xsl:template>
  <xsl:template match="SideNote/Heading">
    <br/>
    <em>
      <xsl:apply-templates/>
    </em>
  </xsl:template>
  <xsl:template match="SideNoteParagraph">
    <br/>
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="Footnote">
    <br/>
    <hr/>
    <span class="required-prodnote">Footnote</span>
    <br/>
    <xsl:apply-templates/>
    <br/>
    <hr/>
    <span class="optional-prodnote">End of footnote</span>
    <br/>
  </xsl:template>
  <xsl:template match="InlineEquation">
    <br/>
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="InlineEquation/Image">
    <img>
      <xsl:attribute name="src">
        <xsl:value-of select="./Image/@src"/>
      </xsl:attribute>
      <xsl:attribute name="alt">
        <xsl:value-of select="./Image/@alt"/>
      </xsl:attribute>
    </img>
  </xsl:template>
  <xsl:template match="InlineEquation/Description">
    <value-of select="text()"/>
    <br/>
  </xsl:template>
  <xsl:template match="InlineEquation/Alternative">
    <value-of select="text()"/>
    <br/>
  </xsl:template>
  <xsl:template match="InlineEquation/TeX | InlineEquation/MathML"/>
  <xsl:template match="AuthorComment">
    <!-- omit -->
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="EditorComment">
    <!-- omit -->
  </xsl:template>
</xsl:stylesheet>
