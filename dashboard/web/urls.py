from django.urls import path
from . import views

urlpatterns = [
    path('', views.dashboard_view, name='dashboard'),
    path('sse/', views.sse_data, name='sse_data'),
]