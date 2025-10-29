<script setup>
import { ref } from 'vue';

// Estado Reactivo
const count = ref(0); // El valor actual del contador
const misFavoritos = ref([]); // El array de números favoritos

// Métodos
const incrementar = () => {
  count.value++;
};

const decrementar = () => {
  count.value--;
};

const reset = () => {
  count.value = 0;
};

const addFavorito = () => {
  // Evitar añadir el mismo número repetidamente
  if (!misFavoritos.value.includes(count.value)) {
    misFavoritos.value.push(count.value);
  }
};

// Elimina un elemento del array por su índice
const deleteFavorito = (index) => {
  misFavoritos.value.splice(index, 1);
};
</script>

<template>
  <div class="flex flex-col items-center justify-center min-h-screen bg-gray-900 text-white p-5">
    <h1 class="text-3xl font-light text-center mb-4">Bryan Motta Bedregal</h1>
    <div class="bg-gray-800 p-8 rounded-xl shadow-2xl w-full max-w-md border border-gray-700">
      
      <h1 class="text-3xl font-light text-center mb-4">Contador Vue 3</h1>
      
      <div class="relative flex justify-center items-center h-24 mb-6"> 
        <Transition name="fade-scale" mode="out-in">
          <p :key="count" 
             class="text-7xl font-extrabold text-center font-mono tracking-wider transition-colors duration-300" 
             :class="{'text-green-400': count > 0, 'text-red-400': count < 0, 'text-white': count === 0}">
            {{ count }}
          </p>
        </Transition>
      </div>

      <div class="flex justify-center space-x-3 mb-10">
        
        <button @click="incrementar" class="bg-green-600 hover:bg-green-700 text-white font-bold py-2 px-4 rounded transition-all duration-200 ease-in-out text-sm active:scale-95">Incrementar</button>
        <button @click="decrementar" class="bg-red-600 hover:bg-red-700 text-white font-bold py-2 px-4 rounded transition-all duration-200 ease-in-out text-sm active:scale-95">Decrementar</button>
        <button @click="reset" class="bg-gray-500 hover:bg-gray-600 text-white font-bold py-2 px-4 rounded transition-all duration-200 ease-in-out text-sm active:scale-95">Reset</button>
        <button @click="addFavorito" class="bg-blue-600 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded transition-all duration-200 ease-in-out text-sm active:scale-95">Añadir</button>
      </div>

      <h2 class="text-xl font-semibold border-b border-gray-600 pb-2 mb-4">Mis Favoritos</h2>
      
      <TransitionGroup name="list" tag="ul" v-if="misFavoritos.length > 0" class="space-y-2">
        <li v-for="(favorito, index) in misFavoritos" 
            :key="favorito"
            class="text-gray-200 bg-gray-700/50 px-4 py-2 rounded-lg text-lg flex justify-between items-center transition duration-150 hover:bg-gray-700 shadow-md"
        >
          <div class="flex items-center space-x-3 justify-between">
            <span class="text-sm text-gray-400 mr-33">#{{ index + 1 }}</span>
            <span class="font-bold text-xl ml-30 pr-2.5 border-r border-gray-400"
            :class="{'ml-33 ':favorito < 10 }"
            >{{ favorito }}</span>
          </div>

          <button 
            @click="deleteFavorito(index)" 
            class="text-red-400 hover:text-red-500 hover:bg-red-400/20 p-1 rounded-full transition-all duration-200 active:scale-90 border"
            title="Eliminar"
          >
            ❌
          </button>
        </li>
      </TransitionGroup>
      
      <p v-else class="text-gray-500 italic text-center py-4">Añade números a tus favoritos.</p>
      
    </div>
  </div>
</template>