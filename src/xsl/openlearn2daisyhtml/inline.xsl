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
  <xsl:template match="InlineEquation">
    <xsl:choose>
      <xsl:when test="string-length(./Image/@src) &gt; 0">
        <img>
          <xsl:attribute name="src">
            <xsl:value-of select="./Image/@src"/>
          </xsl:attribute>
          <xsl:attribute name="alt">
            <xsl:choose>
              <xsl:when test="string-length(Description) &gt; 0">
                <xsl:value-of select="Description"/>
              </xsl:when>
              <xsl:when test="string-length(./Image/@alt) &gt; 0">
                <xsl:value-of select="./Image/@alt"/>
              </xsl:when>
              <xsl:when test="string-length(Alternative) &gt; 0">
                <xsl:value-of select="Alternative"/>
              </xsl:when>
              <xsl:when test="string-length(TeX) &gt; 0">
                <xsl:value-of select="TeX"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>no alternative text</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>
        </img>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="./Image/text() | ./Image/* "/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template
    match="InlineEquation/Description | InlineEquation/Alternative | InlineEquation/TeX | InlineEquation/MathML"/>
  <xsl:template match="InlineFigure">
    <img>
      <xsl:attribute name="src">
        <xsl:value-of select="./Image/@src"/>
      </xsl:attribute>
      <xsl:attribute name="alt">
        <xsl:choose>
          <xsl:when test="string-length(Description) &gt; 0">
            <xsl:value-of select="Description"/>
          </xsl:when>
          <xsl:when test="string-length(./Image/@alt) &gt; 0">
            <xsl:value-of select="./Image/@alt"/>
          </xsl:when>
          <xsl:when test="string-length(Alternative) &gt; 0">
            <xsl:value-of select="Alternative"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>no alternative text</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
    </img>
  </xsl:template>
  <xsl:template match="InlineFigure/Description | InlineFigure/Alternative"/>


  <xsl:template match="InlineChemistry">
    <xsl:choose>
      <xsl:when test="string-length(./Image/@src) &gt; 0">
        <img>
          <xsl:attribute name="src">
            <xsl:value-of select="./Image/@src"/>
          </xsl:attribute>
          <xsl:attribute name="alt">
            <xsl:choose>
              <xsl:when test="string-length(Description) &gt; 0">
                <xsl:value-of select="Description"/>
              </xsl:when>
              <xsl:when test="string-length(./Image/@alt) &gt; 0">
                <xsl:value-of select="./Image/@alt"/>
              </xsl:when>
              <xsl:when test="string-length(Alternative) &gt; 0">
                <xsl:value-of select="Alternative"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>no alternative text</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>
        </img>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="./Image/text() | ./Image/* "/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template
    match="InlineChemistry/Description | InlineChemistry/Alternative"/>
  <xsl:template match="AuthorComment">
    <!-- omit -->
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="EditorComment">
    <!-- omit -->
  </xsl:template>
</xsl:stylesheet>
