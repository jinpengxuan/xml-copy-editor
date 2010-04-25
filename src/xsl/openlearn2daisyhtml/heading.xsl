<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="Heading">
    <xsl:choose>
      <xsl:when test="count(ancestor-or-self::SubSubSection) &gt; 0">
        <h5>
          <xsl:call-template name="apply-templates-lang"/>
        </h5>
      </xsl:when>
      <xsl:when test="count(ancestor-or-self::SubSection) &gt; 0">
        <h4>
          <xsl:call-template name="apply-templates-lang"/>
        </h4>
      </xsl:when>
      <xsl:when test="count(ancestor-or-self::Section) &gt; 0">
        <h3>
          <xsl:call-template name="apply-templates-lang"/>
        </h3>
      </xsl:when>
      <xsl:when test="count(ancestor-or-self::Session) &gt; 0 or count(//ancestor-or-self::Introduction) &gt; 0">
        <h2>
          <xsl:call-template name="apply-templates-lang"/>
        </h2>
      </xsl:when>
      <xsl:otherwise>
        <h1>
          <xsl:call-template name="apply-templates-lang"/>
        </h1>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="SubHeading">
    <xsl:choose>
      <xsl:when test="count(ancestor-or-self::SubSubSection) &gt; 0">
        <h6>
          <xsl:call-template name="apply-templates-lang"/>
        </h6>
      </xsl:when>
      <xsl:when test="count(ancestor-or-self::SubSection) &gt; 0">
        <h5>
          <xsl:call-template name="apply-templates-lang"/>
        </h5>
      </xsl:when>
      <xsl:when test="count(ancestor-or-self::Section) &gt; 0">
        <h4>
          <xsl:call-template name="apply-templates-lang"/>
        </h4>
      </xsl:when>
      <xsl:when test="count(ancestor-or-self::Session) &gt; 0 or count(//ancestor-or-self::Introduction) &gt; 0">
        <h3>
          <xsl:call-template name="apply-templates-lang"/>
        </h3>
      </xsl:when>
      <xsl:otherwise>
        <h2>
          <xsl:call-template name="apply-templates-lang"/>
        </h2>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="SubSubHeading">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Question">
    <xsl:if test="count(Heading) = '0'">
      <p>
        <span class="optional-prodnote">Question</span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Answer">
    <xsl:if test="count(Heading) = '0'">
      <p>
        <span class="optional-prodnote">Answer</span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Discussion">
    <xsl:if test="count(Heading) = '0'">
      <p>
        <span class="optional-prodnote">Discussion</span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
</xsl:stylesheet>
