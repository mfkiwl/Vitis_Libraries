.. 
   .. Copyright © 2019–2023 Advanced Micro Devices, Inc

.. `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. meta::
   :keywords: Garman-Kohlhagen, Closed-Form, Foreign Exchange, FX
   :description: The Garman Kohlhagen model is used to price Foreign Exchange (FX) Options. An FX Option involves the right to exchange money in one currency into another currency at an agreed exchange rate on a specified date. 
   :xlnxdocumentclass: Document
   :xlnxdocumenttype: Tutorials


*************************************
Garman-Kohlhagen Closed-Form Solution
*************************************

.. toctree::
   :maxdepth: 1

Overview
========

The Garman Kohlhagen model is used to price Foreign Exchange (FX) Options. An FX Option involves the right to exchange money in one currency into another currency at an agreed exchange rate on a specified date. The formula for calculating a call option is:

.. math::
   c = S.\mathrm{e}^{-r_fT}N(d1) - K.\mathrm{e}^{-r_dT}N(d2)

.. math::
    d1 = \frac{ln\left(\frac{S}{K}\right)+\left(r_d - r_f + \frac{\sigma^2}{2}\right)T}{\sigma\sqrt{T}}

.. math::
    d2 = \frac{ln\left(\frac{S}{K}\right)+\left(r_d - r_f - \frac{\sigma^2}{2}\right)T}{\sigma\sqrt{T}} = d1 - \sigma\sqrt{T}

where:

c = call price

S = spot exchange rate

K = strike exchange rate

:math:`r_d` = domestic interest rate

:math:`r_f` = foreign interest rate

:math:`N()` = cumulative standard normal distribution function

:math:`T` = time to maturity

:math:`\sigma` = exchange rate volatility

The form of this equation is the same as the Black Scholes Merton closed form solution with the following substitutions:

r, the risk free interest rate is replaced by :math:`r_d`, the domestic interest rate

q, the dividend yield is replaced by :math:`r_f`, the foreign interest rate

References
==========

.. [GK.1] Mark B. Garman and Steven W. Kohlhagen (1983), “Foreign Currency Option Values”, Journal of International Money and Finance

